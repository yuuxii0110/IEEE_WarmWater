const express = require('express');
const app = express();
const http = require('http');
const server = http.createServer(app);
const { Server } = require("socket.io");
const io = new Server(server);
const mqtt = require('mqtt');
const session = require('express-session');
const bodyParser = require("body-parser");
const router = express.Router();
var client_device_map = new Map();
var client_credit_map = new Map();
var urlencodedParser = bodyParser.urlencoded({ extended: false })

app.use("/", router);
app.use("/WarmWater", express.static(__dirname + '/frontend'));

router.use(session({
  secret: '69276e87-ad4f-45a5-9665-0a7d72905624',
  resave: false,
  saveUninitialized: true,
  cookie: { secure: false, maxAge: 3600000 }
}));

function subscribe_mqtt_topics(){
  client.subscribe("connection_established");
  client.subscribe("disconnect_device");
  console.log("mqtt subscribed")
}

function calculate_credit(time, energy, weightage=1){
    return Math.round(weightage*(time*0.25 + energy*0.05));
}

function register_if_not_done(user_id){
    //initialize client credit if never logged in
    if(!client_credit_map.get(user_id)){
        client_credit_map.set(user_id, 0);
    }
}

function PairDevices(user_id, device_id){
    const topic = "/pair_request/" + device_id;
    const message = "1/" + user_id;
    //send to esp32
    client.publish(topic, message);
    //send to webpage
    io.emit(user_id + '_connected', "1");
}

const client = mqtt.connect('mqtt://127.0.0.1:1883');
subscribe_mqtt_topics();

link = '/WarmWater';
app.get(link,function(req,res){    
    if(req.session.session_started){
        res.sendFile(__dirname + '/frontend/workout.html');
    }
    else if(req.session.logged_in){
        res.sendFile(__dirname + '/frontend/main.html');
    }
    else if(!req.session.logged_in){
        res.sendFile(__dirname + '/frontend/home.html');
    } 
    else{
        res.sendFile(__dirname + '/frontend/login.html');
    }
});



client.on("message", function (topic, payload){
    if (topic == "connection_established") {
        let details = payload.split("/");
        let user_id = details[0];
        let device_id = details[1];
        client_device_map.set(user_id, device_id);
        console.log(user_id + " connected to device: " + device_id);
    }

    else if(topic == "disconnect_device"){
        let details = payload.split("/");
        let user_id = details[0];
        let device_id = details[1];
        let time = details[2];
        let energy = details[3];
        let earned_credit = calculate_credit(time,energy);
        let previous_credit = parseInt(client_credit_map.get(user_id));
        client_credit_map.set(user_id,earned_credit+previous_credit)
        io.emit(user_id + '_disconnected', credit);
        client_device_map.delete(user_id);
    }
});

router.post("/workout_stopped",urlencodedParser, (req, res) => {
    let user_id = req.body.username;
    let device_id = req.body.device;
    req.session.session_started = null;
});

router.post("/from_device_connection_request",urlencodedParser, (req, res) => {
    let user_id = req.body.username;
    let device_id = req.body.device;
    setTimeout(PairDevices, 2000, user_id, device_id);    
    req.session.session_started = true;
    res.end("1");
});

router.post("/from_device_disconnect_request",urlencodedParser, (req, res) => {
    let user_id = req.body.username;
    let device_id = req.body.device;
    const topic = "/pair_request/" + device_id;
    const message = "0/"+user_id;
    client.publish(topic, message);
    io.emit(user_id + '_disconnected', credit); 
});

router.post('/log_in',urlencodedParser, (req, res) => {
    let user_id = req.body.username;
    let pw = req.body.password;
    console.log(user_id, pw)
    if(user_id == "warmwater"){
        register_if_not_done(user_id);
        req.session.logged_in = true;
        res.end("1");
    }
    else{
        res.end("0");
    }
});

router.post('/session_end',urlencodedParser, (req, res) => {
    let user_id = req.body.username;
    register_if_not_done(user_id);
    let time = req.body.time;
    let energy = req.body.energy;
    let earned_credit = calculate_credit(time,energy);
    let previous_credit = parseInt(client_credit_map.get(user_id));
    client_credit_map.set(user_id,earned_credit+previous_credit)
    res.end(credit);
    client_device_map.delete(user_id);
});

router.post('/request_available_credit',urlencodedParser, (req, res) => {
    let user_id = req.body.username;
    register_if_not_done(user_id);
    res.end(client_credit_map.get(user_id))
});

server.listen(8080, () => {
});

client.on('connect', function () {
    console.log("web client connected")
 }
)
