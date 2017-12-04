<template>
  <div class="devices">
    <div class="device-grid" >
        <device v-for="(device, id) in devices" :id="id" :status="device.status" :temperature="device.temperature" :moisture="device.moisture" :humidity="device.humidity"></device>

    </div>
  </div>
</template>

<script>
import Device from './Device'
import axios from 'axios'

export default {
  name: 'Devices',
  data () {
    return {
        refresh_timer: '',
        devices: {}
    }
  },
  methods: {
      getDevices() {
          this.getDevicesFromBackend()
              .then( response => {
                  this.devices = response.data
              })
              .catch( error => {
                  console.log( error )
              })
      },

      updateDevices() {
          this.getDevicesFromBackend()
              .then( response => {
                  for( var key in response ) {
                      if( this.devices.hasOwnProperty( key ) ) {
                         this.devices.key = response.key;
                      }
                      else {
                          this.$set( this.devices, key, response[ key ] );
                      }
                  }
              })
              .catch( error => {
                  console.log( error )
              })
          
      },

      getDevicesFromBackend() {
          const path = 'http://192.168.1.146:5000/api/devices';
          return axios.get( path )
      }
  },

    created: function()
    {
        this.getDevices();
        this.refresh_timer = setInterval( this.getDevices, 5000 );
    },

  components: {
      device: Device
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h1, h2 {
  font-weight: normal;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}

.device-grid-old {

    display: grid;
  grid-template-columns: repeat(3, 1fr);
  grid-gap: 20px;
  grid-auto-rows: minmax(400px, auto);
  justify-content:center;
  align-content: end;
}

.device-grid {
     display:flex;
     flex-direction:row;
     flex-wrap:wrap;
}
</style>
