const express = require('express')
const path = require('path')
const cors = require('cors')
//import libraires

const app = express()

app.use(express.static(__dirname + '/html'))
app.use(cors())
app.use(express.json())
app.use(express.urlencoded({ extended: true }))

//get method template for futher features
app.get('/temp', (req, res) => {
    res.statusCode = 200;
    res.send({ "status": "estoy bien" })

})

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
    console.log(temperature +" "+ tempTime)

    res.send({
        "id":sensorId,
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
//this app is running on port 8080 in order to integrate with docker
app.listen(8080, function () {
    console.log('app is running in http://localhost:8080')
})



