const express = require('express')
const path = require('path')
const cors = require('cors')
const data = require('./temperatureData')
const Statistics = require('statistics.js');
//const Plotly = require('plotly.js-dist');
var AWS = require('aws-sdk');

var s3 = new AWS.S3();

// Los nombres de buckets deben ser únicos entre todos los usuarios de S3

// var myBucket = 'my.unique.bucket.name';

// var myKey = 'myBucketKey'

// var ddb = new AWS.DynamoDB({apiVersion: '2012-08-10'});

// var params = {
//   ExpressionAttributeValues: {
//     ':s': {N: '2'},
//     ':e' : {N: '09'},
//     ':topic' : {S: 'PHRASE'}
//   },
//   KeyConditionExpression: 'Season = :s and Episode > :e',
//   ProjectionExpression: 'Episode, Title, Subtitle',
//   FilterExpression: 'contains (Subtitle, :topic)',
//   TableName: 'EPISODES_TABLE'
// };

// ddb.query(params, function(err, data) {
//   if (err) {
//     console.log("Error", err);
//   } else {
//     //console.log("Success", data.Items);
//     data.Items.forEach(function(element, index, array) {
//       console.log(element.Title.S + " (" + element.Subtitle.S + ")");
//     });
//   }
// });

//import libraires

const app = express()

app.use(express.static(__dirname + '/html'))
app.use(cors())
app.use(express.json())
app.use(express.urlencoded({ extended: true }))

//get method template for futher features
app.get('/temp', (req, res) => {
    res.statusCode = 200;
    res.json(data);

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
// let array = data.coldChain.temperature
// for(i in array){
//     console.log(array[i])
// }


app.listen(8080, function () {
    console.log('app is running in http://localhost:8080')
})



