const express = require('express')
const path = require('path')
const cors = require('cors')
const data = require('./temperatureData')


let array = data.temperature
let lista = []
for(i in data){
    lista.push(data[i].temperature);
}
console.log(lista)