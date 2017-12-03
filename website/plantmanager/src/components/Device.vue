<template>
  <div class="device">
      <h1>{{ id }}</h1>
      <span>{{ id }}:{{status}}</span>
  </div>
</template>

<script>
export default {
  name: 'Device',
  props: [ 'id', 'status' ]
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
</style>
