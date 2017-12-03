<template>
  <div id="app">
      <span ><img id="header-image" src="./assets/logo.png" /></span>
    <span id="header">
        <h4>By Samantha, Dolapo, and Owen</h4>
    </span>
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
  font-family: 'Roboto', 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  padding:45px;
  display:flex;
  flex-direction:column;
  justify-content: space-between;

}
#header {
    font-family: 'Quicksand';
    color:#E1E2E1;

}
#header-image{
     height:150px;
}
</style>
