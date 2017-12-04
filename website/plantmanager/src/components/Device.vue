<template>
  <div class="device">
      <h1>{{ id }}</h1>
      <h4>{{ status }}</h4>
      <div id="#vals" >
          <div class="val">Current moisture level: {{ moisture }}</div>
          <div class="val">Current humidity level: {{ humidity }}</div>
          <div class="val">Current temperature: {{ temperature }}</div>
      </div>
  </div>
</template>

<script>
export default {
  name: 'Device',
  props: [ 'id', 'status', 'moisture', 'humidity', 'temperature' ]
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

.device {
    width:400px;
    height:400px;
     background-color:#E1E2E1;
     padding:45px;
     margin:20px;
}

.val {
     text-align: left;
     font-size: 25px;
     padding-top: 10px;
     padding-bottom: 10px;
}
#vals {
     display:flex;
     flex-direction:column;
     justify-content:left;
     justify-content:left;
     align-content:left;
}
</style>
