// To Do
// 1. When start session button is pressed, send a req to server
// 2. When end session button is pressed, req for token earned

/** Stores the reference to the elapsed time interval*/
var elapsedTimeIntervalRef;

/** Stores the start time of timer */
var startTime;

/** Stores the details of elapsed time when paused */
var elapsedTimeWhenPaused;

function startTimer() {
    // Create a new data object
    startTime = new Date();
    var initTime = 0;
    var timetotal = 100;

    elapsedTimeIntervalRef = setInterval(() =>{
        elapsedTimeText.innerText = timeAndDateHandling.getElapsedTime(startTime);
        var progressBarWidth = initTime * prog_bar.width / timetotal;
        prog_bar.width = progressBarWidth;
        initTime ++;

    }, 1000);
}

function stopTimer() {
    if (typeof elapsedTimeIntervalRef !== "undefined") {
        clearInterval(elapsedTimeIntervalRef);
        elapsedTimeIntervalRef = undefined;
    }
}   

function stop_session() {
    console.log("End session");

    btn.style.display = "none";

    instruction.style.display = "inline-block";
    bicycleImg.src = "images/cycling-still.png";

    stopTimer();

}

var timeAndDateHandling = {
    /** Computes the elapsed time since the moment the function is called in the format mm:ss or hh:mm:ss
     * @param {String} startTime - start time to compute the elapsed time since
     * @returns {String} elapsed time in mm:ss format or hh:mm:ss format if elapsed hours are 0.
     */
    getElapsedTime: function (startTime) {

        // Record end time
        let endTime = new Date();

        // Compute time difference in milliseconds
        let timeDiff = endTime.getTime() - startTime.getTime();

        // Convert time difference from milliseconds to seconds
        timeDiff = timeDiff / 1000;

        // Extract integer seconds that dont form a minute using %
        let seconds = Math.floor(timeDiff % 60); //ignoring uncomplete seconds (floor)

        // Pad seconds with a zero if neccessary
        let secondsAsString = seconds < 10 ? "0" + seconds : seconds + "";

        // Convert time difference from seconds to minutes using %
        timeDiff = Math.floor(timeDiff / 60);

        // Extract integer minutes that don't form an hour using %
        let minutes = timeDiff % 60; //no need to floor possible incomplete minutes, becase they've been handled as seconds

        // Pad minutes with a zero if neccessary
        let minutesAsString = minutes < 10 ? "0" + minutes : minutes + "";

        // Convert time difference from minutes to hours
        timeDiff = Math.floor(timeDiff / 60);

        // Extract integer hours that don't form a day using %
        let hours = timeDiff % 24; //no need to floor possible incomplete hours, becase they've been handled as seconds

        // Convert time difference from hours to days
        timeDiff = Math.floor(timeDiff / 24);

        // The rest of timeDiff is number of days
        let days = timeDiff;

        let totalHours = hours + (days * 24); // add days to hours
        let totalHoursAsString = totalHours < 10 ? "0" + totalHours : totalHours + "";

        if (totalHoursAsString === "00") {
            return minutesAsString + ":" + secondsAsString;
        } else {
            return totalHoursAsString + ":" + minutesAsString + ":" + secondsAsString;
        }
    }
}

// function progress(start_time, timetotal, $element) {
//     var progressBarWidth = start_time * $element.width() / timetotal;
//     $element.find('div').animate({ width: progressBarWidth }, 500).html(Math.floor(start_time/60) + ":"+ start_time%60);
// }


