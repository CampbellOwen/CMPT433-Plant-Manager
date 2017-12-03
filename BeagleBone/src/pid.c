// CMPT 433: Project
// References:
// - "PID Without PhD" by Tim Wescott
//     - http://www.wescottdesign.com/articles/pid/pidWithoutAPhd.pdf
// - "A Method for Precision Closed-loop Irrigation Using a Modified PID Control Algorithm" by Martin Goodchild and Malcolm Jenkins
//     - http://www.sensorsportal.com/HTML/DIGEST/may_2015/Vol_188/P_2662.pdf

#include <include/pid.h>
#include <include/device_manager.h>
#include <include/device_array.h>
#include <include/udp_server.h>
#include <include/sqlite3.h>
#include <include/define.h>

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#define INTEGRATOR_MAX 40
#define INTEGRATOR_MIN 20

#define PROPORTIONAL_GAIN 10
#define DERIVATIVE_GAIN 1000

// A good starting value for the integrator gain is:
// Smaller than the proportional gain by the same ratio as proportional gain to derivative gain
#define INTEGRATOR_GAIN 0.01
#define MOISTURE_GOAL 30

#define DB_NAME "plants.db"
#define SQL_STATEMENT_BUFFER_SIZE 1024
#define SELECT_LAST_PID "SELECT * FROM pid WHERE id=(SELECT MAX(TIME) FROM pid);"
#define INSERT_PID "INSERT INTO pid (id, time, derivativeState, integratorState) VALUES ( %u, %llu, %f, %f );"

#define CATEGORY_ACTION 'A'
#define ACTION_PUMP 'p'

#define SLEEP_TIME_SEC 5

static sqlite3* db;

static int sql_read_pid_callback( void* ret_args, int num_rows, char** rows, char** columns )
{
     pid_callback_args_t* args = (pid_callback_args_t*) ret_args;

     pid_row_t* row = malloc( sizeof( pid_row_t ) );

     row->id = atoi( rows[ 0 ] );
     row->timestamp = atol( rows[ 1 ] );
     row->derivativeState = atof( rows[ 2 ] );
     row->integratorState = atof( rows[ 3 ] );

     if( args->length >= args->max ) {
          printf( ERROR "Too many rows returned\n" );
          free( row );
          return 1;
     }
     else {
          args->rows[ args->length++ ] = *row;
     }
     return 0;
}

static pid_row_t* PID_GetLastPID()
{
  char sql[ SQL_STATEMENT_BUFFER_SIZE ];
  sprintf( sql, SELECT_LAST_PID );
  char* err_msg = NULL;

  pid_callback_args_t args;
  args.max = 1;
  args.length = 0;
  args.rows = malloc( args.max * sizeof( pid_row_t ) );

  int ret = sqlite3_exec( db, sql, sql_read_pid_callback, &args, &err_msg );
  if( ret != SQLITE_OK )
  {
        fprintf( stderr, ERROR "Error reading from PID table: %s\n", err_msg );
        sqlite3_free( err_msg );
        return NULL;
  }
  else {
      printf( INFO "Last PID was succesfully read from db\n" );
  }

  return args.rows;
}

void PID_SavePIDdata( device_t* device, float derivativeState, float integratorState)
{
     long long curr_time = ( long long )time( NULL );
     uint32_t id = device->id;
     char sql_statement[ SQL_STATEMENT_BUFFER_SIZE ];

     printf( INFO "Saving PID data from id: %u, derivativeState: %f, integratorState: %f\n", id, derivativeState, integratorState );
     sprintf( sql_statement, INSERT_PID, id, curr_time, derivativeState, integratorState );

    char* err_msg = NULL;

    int ret = sqlite3_exec( db, sql_statement, NULL, NULL, &err_msg );
    if( ret != SQLITE_OK ) {
          fprintf( stderr, ERROR "Error writing to PID table: %s\n", err_msg );
          sqlite3_free( err_msg );
    }
    else {
        printf( INFO "Values succesfully stored in db\n" );
    }
}

void PID_Update(device_t* device)
{
  moisture_row_t* lastMoisture = DeviceManager_GetLastMoisture(device);
  if (lastMoisture == NULL) {
    return;
  }

  float pTerm, dTerm, iTerm;
  float error = MOISTURE_GOAL - lastMoisture->value;
  pTerm = PROPORTIONAL_GAIN * error; // calculate the proportional term

  pid_row_t* lastPID = PID_GetLastPID();
  if (lastPID == NULL) {
    return;
  }

  float integratorState;

  // Calculate the integral state with appropriate limiting
  if (!isnan(lastPID->integratorState)) {
    integratorState = lastPID->integratorState + error;
  }
  else {
    integratorState = error;
  }

  // Limit the integrator state if necessary
  if (integratorState > INTEGRATOR_MAX)
  {
    integratorState = INTEGRATOR_MAX;
  }
  else if (integratorState < INTEGRATOR_MIN)
  {
    integratorState = INTEGRATOR_MIN;
  }

  // calculate the integral term
  iTerm = INTEGRATOR_GAIN * integratorState;

  // calculate the derivative
  dTerm = DERIVATIVE_GAIN * (lastPID->derivativeState - lastMoisture->value);
  float derivativeState = lastMoisture->value;

  PID_SavePIDdata(device, derivativeState, integratorState);
  DeviceManager_ActivatePump(device, round(pTerm + dTerm + iTerm));
}

_Bool PID_Init(void)
{
  int ret = sqlite3_open( DB_NAME, &db );
  if( ret )
  {
    fprintf( stderr, ERROR "Can't open database: %s\n", sqlite3_errmsg( db ) );
    return 0;
  }

  return 1;
}

void PID_Shutdown(void)
{
  sqlite3_close(db);
}
