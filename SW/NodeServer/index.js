const express = require('express')
const path = require('path')
const cors = require('cors')
//const data = require('./temperatureData')

var XMLHttpRequest = require("xmlhttprequest").XMLHttpRequest;

//import libraires

const app = express()

app.use(express.static(__dirname + '/html'))
app.use(cors())
app.use(express.json())
app.use(express.urlencoded({
    extended: true
}))

function minusDate(fechaF, fechaI) {
    let fechaNum1 = new Date(fechaF.Timestamp);
    let fechaNum2 = new Date(fechaI.Timestamp);
    let x = fechaNum1 - fechaNum2;
    if (x < 0) {
        return -1;
    }
    if (x > 0) {
        return 1;
    }
    // a debe ser igual b
    return 0;
}

function sortDate(arr, a) {
    let arrT;
    let temp = [];
    if (a) {
        arrT = arr.sort(minusDate);
        arrT.splice(0, 2);

    } else {
        arrT = arr.sort(minusDate);
    }
    for (let i = 0; i < arrT.length; i += 10) {
        temp.push(arrT[i]);
    }

    return temp;
}

//let x = document.getElementById("id").value;

//get method template for futher features
app.get('/temp', (req, res) => {
    let id = req.query.id;
    if (id == ""||id.length<4) {
        //alert("id invalido");
        res.status(401).send("id invalido");
    } else {
        
        let jstm = JSON.parse(httpGet('https://ij60i8kpw0.execute-api.us-east-1.amazonaws.com/prod/api/coldchain/'+id));
        
        let sensor1 = sortDate(jstm['0x48'], 1);
        let sensor2 = sortDate(jstm['0x49']);
        let sensor3 = sortDate(jstm['0x4A']);
        let sensor4 = sortDate(jstm['0x4B']);
        let sensor5 = sortDate(jstm['0x4C']);
        let sensor6 = sortDate(jstm['0x4D']);
        let sensor7 = sortDate(jstm['0x4E']);
        let sensor8 = sortDate(jstm['0x4F'], 1);

        let sensorsList = {
            sensor1,
            sensor2,
            sensor3,
            sensor4,
            sensor5,
            sensor6,
            sensor7,
            sensor8
        };

        res.status(200).send(JSON.stringify(sensorsList));
    }

})
app.get('/travels', (req, res) => {
    //console.log(x);
    let jstr = httpGetTravels();
    //let tempTime = req.body.Temperature;
    res.statusCode = 200;
    res.send(jstr);

})

function httpGet(theUrl) {
    let xhr = new XMLHttpRequest();
    xhr.open("GET", theUrl, false);
    xhr.send(null);
    return xhr.responseText;
}

function httpGetTravels() {
    let xhr = new XMLHttpRequest();
    xhr.open("GET", 'https://ij60i8kpw0.execute-api.us-east-1.amazonaws.com/prod/api/coldchain/travels', false);
    xhr.send(null);
    return xhr.responseText;
}

//post method that will process the data from sensors and convert it to a json
app.post('/', (req, res) => {
    let tempTime = req.body.date;
    let sensorId = req.body.sensorId;
    let temperature = parseInt(req.body.temp);
    let timeSplit = tempTime.split(" ");
    let day = timeSplit[0].replace(',', '');
    let month = timeSplit[1];
    let dayM = timeSplit[2];
    let year = timeSplit[3];
    let hour = timeSplit[4];
    //let min = timeSplit[5];
    //let sec = timeSplit[6];
    console.log(temperature + " " + tempTime)

    res.send({
        "id": sensorId,
        "temp": temperature,
        "date": {
            "day": day,
            "month": month,
            "day of month": dayM,
            "year": year,
            "hour": hour,
            // "minute":min,
            //"second":sec      
        }
    })
})

// {  	
//     "id":"0x48",
//    "temp":"25",
//   "date":"Monday, March 22 2021 19:39:17"
// }
// let array = data.coldChain.temperature
// for(i in array){
//     console.log(array[i])
// }


app.listen(8080, function () {
    console.log('app is running in http://localhost:8080')
})