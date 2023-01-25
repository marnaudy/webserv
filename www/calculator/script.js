function convertSymbolToString(sym) {
    switch (sym) {
        case '+':
            return 'plus';
        case '-':
            return 'minus';
        case '*':
            return 'multiply';
        case '/':
            return 'divide';
    }
}


//Select display and add event listeners to the buttons
const display = document.querySelector('#display');
const numbers = document.querySelectorAll('.number');
const operators = document.querySelectorAll('.operator');
numbers.forEach(number => number.addEventListener('click', clickNumber));
numbers.forEach(number => number.addEventListener('transitionend', removeTransition));
operators.forEach(operator => operator.addEventListener('click', clickOperator));
operators.forEach(operator => operator.addEventListener('transitionend', removeTransition));
const enterButton = document.querySelector('#enter');
const clearButton = document.querySelector('#clear');
const backButton = document.querySelector('#back');
const decimalButton = document.querySelector('#decimal');
enterButton.addEventListener('click', enter);
enterButton.addEventListener('transitionend', removeTransition);
clearButton.addEventListener('click', clear);
clearButton.addEventListener('transitionend', removeTransition);
backButton.addEventListener('click', back);
backButton.addEventListener('transitionend', removeTransition);
decimalButton.addEventListener('click', addDecimal);
decimalButton.addEventListener('transitionend', removeTransition);
window.addEventListener('keydown', keyPress);


function clickNumber(e) {
    e.target.classList.add('pressed');
    newNumber(e.target.textContent);
}

function newNumber(str) {
    calculation[calculation.length - 1] += str;
    updateDisplay();
}

function clickOperator(e) {
    e.target.classList.add('pressed');
    newOperator(e.target.textContent);
}

function newOperator(str) {
    calculation.push(str);
    calculation.push('');
    updateDisplay();
}

function enter() {
    const bttn = document.querySelector('#enter');
    bttn.classList.add('pressed');
    calculation = calculate(calculation);
    updateDisplay();
}

function clear() {
    const bttn = document.querySelector('#clear');
    bttn.classList.add('pressed');
    calculation = [''];
    updateDisplay();
}

function back() {
    const bttn = document.querySelector('#back');
    bttn.classList.add('pressed');
    if (calculation.length >1 || calculation[0]) { //Checks that the calculation array isn't empty
        if (calculation[calculation.length -1] === '') {
            calculation.pop();
            calculation.pop();
        } else {
            calculation[calculation.length -1] = calculation[calculation.length -1].slice(0,-1);
        }
        updateDisplay();
    }
}

function addDecimal() {
    if (calculation[calculation.length-1] === '') {
        calculation[calculation.length-1] = '0.';
        updateDisplay();
        return;
    }
    if (calculation[calculation.length-1].indexOf('.') === -1) {
        calculation[calculation.length-1] += '.';
        const bttn = document.querySelector('#decimal');
        bttn.classList.add('pressed');
        updateDisplay();
    }
}

function updateDisplay() {
    display.textContent = calculation.reduce((concat, str) => (concat + str), '');
    if (display.textContent.length < 9) {
        display.style.fontSize = '80px';
    } else if (display.textContent.length < 14) {
        display.style.fontSize = '50px';
    } else {
        display.style.fontSize = '30px';
    }
}

function keyPress(e) {
    let button;
    switch (e.key) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            bttn = document.querySelector(`#b`+e.key);
            bttn.classList.add('pressed');
            newNumber(e.key);
            break;
        case '+':
        case '-':
        case '*':
        case '/':
            bttn = document.querySelector(`#${convertSymbolToString(e.key)}`);
            bttn.classList.add('pressed');
            newOperator(e.key);
            break;
        case 'Enter':
            enter();
            break;
        case 'Backspace':
            back();
            break;
        case '.':
            addDecimal();
            break;
        case 'Escape':
            clear()
            break;
    }
}

function removeTransition(e) {
    e.target.classList.remove('pressed');
}

isPriorityOperation = (str) => (str === '/' || str === '*');
// Returns true is it is a high priority calculation

isLowPriorityOperation = (str) => (str === '+' || str === '-');

function calculate(calculation) {
    if (calculation.some(str => (str === '') || str.search(/[a-zA-Z]/)!= -1 ) ) {
        return ['ERROR'];
    }
    while (calculation.some(isPriorityOperation)) {
        const index = calculation.findIndex(isPriorityOperation);
        let result;
        if (calculation[index] === '*') {
            result = calculation[index-1] * calculation[index+1];
        } else {
            if (parseInt(calculation[index+1],10) === 0) {
                return ['Forbidden !']
            }
            result = calculation[index-1] / calculation[index+1];
        }
        result = Math.round(result*100000)/100000;
        calculation.splice(index-1, 3, result.toString());
    }
    while (calculation.some(isLowPriorityOperation)) {
        const index = calculation.findIndex(isLowPriorityOperation);
        let result;
        if (calculation[index] === '+') {
            result = +calculation[index-1] + +calculation[index+1];
        } else {
            result = calculation[index-1] - calculation[index+1];
        }
        result = Math.round(result*100000)/100000;
        calculation.splice(index-1, 3, result.toString());
    }
    return calculation;
}



let calculation = [''];