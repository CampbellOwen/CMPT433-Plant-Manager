<template>
  <div id="app">
    <img src="./assets/logo.png">
    <div id="header">
        <h1>Plant Manager</h1>
        <h2>By Samantha, Dolapo, and Owen</h2>
    </div>
    <Devices/>
  </div>
</template>

<script>
import Devices from './components/Devices'

export default {
  name: 'app',
  components: {
    Devices
  }
}
</script>

<style>
body {
    background-color: #424242;
}
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
  padding:45px;
  display:flex;
  flex-direction:column;
}
#header {
     background-color:#E1E2E1;

}
</style>
