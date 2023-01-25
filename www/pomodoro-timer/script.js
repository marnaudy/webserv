const timerMinutes = document.querySelector('#time-display-minutes');
const timerSeconds = document.querySelector('#time-display-seconds');
const sessionSetting = document.querySelector('#session-display');
const breakSetting = document.querySelector('#break-display');
const currentState = document.querySelector('#current-state');


const buttons = document.querySelectorAll('button', 'i');
buttons.forEach(bttn => bttn.addEventListener('click', function (e) {
    if (e.target.nodeName === 'I') {
        e.target.id = e.target.parentElement.id;
    }
    switch (e.target.id) {
        case 'start':
            start();
            break;
        case 'reset':
            reset();
            break;
        case 'pause':
            pause();
            break;
        case 'stop':
            stop();
            break;
        case 'session-up':
            minutesUp();
            break;
        case 'session-down':
            minutesDown();
            break;
        case 'break-up':
            breakUp();
            break;
        case 'break-down':
            breakDown();
            break;
    }
}));
function minutesUp() {
    if (timerIsRunning) return;
    sessionSetting.textContent = parseInt(sessionSetting.textContent, 10) + 1;
    if (session) {
        seconds = 0;
        minutes = parseInt(sessionSetting.textContent, 10);
        updateDisplay();
    }
}
function minutesDown() {
    if (timerIsRunning) return;
    if (parseInt(sessionSetting.textContent, 10) === 1) return;
    sessionSetting.textContent = parseInt(sessionSetting.textContent, 10) - 1;
    if (session) {
        seconds = 0;
        minutes = parseInt(sessionSetting.textContent, 10);
        updateDisplay();
    }
}
function breakUp() {
    if (timerIsRunning) return;
    breakSetting.textContent = parseInt(breakSetting.textContent, 10) + 1;
    if (!session) {
        seconds = 0;
        minutes = parseInt(breakSetting.textContent, 10);
        updateDisplay();
    }
}
function breakDown() {
    if (timerIsRunning) return;
    if (parseInt(breakSetting.textContent, 10) === 1) return;
    breakSetting.textContent = parseInt(breakSetting.textContent, 10) - 1;
    if (!session) {
        seconds = 0;
        minutes = parseInt(breakSetting.textContent, 10);
        updateDisplay();
    }
}
function updateDisplay() {
    timerMinutes.textContent = minutes;
    if (seconds < 10) {
        timerSeconds.textContent = '0' + seconds;
    } else {
        timerSeconds.textContent = seconds;
    }
    if (session) {
        currentState.textContent = 'Session';
    } else {
        currentState.textContent = 'Break';
    }
}

function decreaseTimer() {
    if (seconds === 0) {
        if (minutes === 0 && session) {
            minutes = parseInt(breakSetting.textContent, 10);
            session = false;
            const audio = document.querySelector('#beep');
            audio.play();
        } else if (minutes === 0 && !session) {
            minutes = parseInt(sessionSetting.textContent, 10);
            session = true;
            const audio = document.querySelector('#beep');
            audio.play();
        } else {
            minutes--;
            seconds = 59;
        }
    } else {
        seconds--;
    }
    updateDisplay();
}

function start() {
    if (timerIsRunning) return;
    interval = window.setInterval(decreaseTimer, 1000);
    timerIsRunning = true;
}

function reset() {
    sessionSetting.textContent = '25';
    breakSetting.textContent = '5';
    stop();
}

function pause() {
    window.clearInterval(interval);
    timerIsRunning = false;
}

function stop() {
    window.clearInterval(interval);
    minutes = parseInt(sessionSetting.textContent, 10);
    seconds = 0;
    timerIsRunning = false;
    session = true;
    updateDisplay();
}

let minutes = 25;
let seconds = 0;
let interval = null;
let timerIsRunning = false;
let session = true;