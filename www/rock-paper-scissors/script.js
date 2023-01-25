function random(number) { //Returns an integer n such that 0<=n<number
    return Math.floor(Math.random()*number);
}

function computerPlay() {
    let possibleMoves = ['Rock', 'Paper', 'Scissors'];
    return possibleMoves[random(3)];
}

function updateScore() {
    score.textContent = `Player : ${playerScore} || Computer : ${computerScore}`;
}

function playRound(e) {
    if (computerScore+playerScore>=5) {
        result.style.color = 'black';
        result.textContent = `Please start a new game`;
        return;
    }
    let playerSelection = e.target.id;
    let computerSelection = computerPlay();
    switch (playerSelection) {
        case 'rock':
            switch (computerSelection) {
                case 'Rock':
                    result.textContent = `Tie! You both chose Rock!`;
                    result.style.color = 'black';
                    break;
                case 'Paper':
                    computerScore ++;
                    result.textContent = `You lose! Paper beats Rock`;
                    result.style.color = 'red';
                    break;
                case 'Scissors':
                    playerScore ++;
                    result.textContent = `You win! Rock beats Scissors`;
                    result.style.color = 'green';
                    break;
            }
            break;
        case 'paper':
            switch (computerSelection) {
                case 'Rock':
                    playerScore ++;
                    result.textContent = `You win! Paper beats Rock`;
                    result.style.color = 'green';
                    break;
                case 'Paper':
                    result.textContent = `Tie! You both chose Paper!`;
                    result.style.color = 'black';
                    break;
                case 'Scissors':
                    computerScore ++;
                    result.textContent = `You lose! Scissors beat Paper`;
                    result.style.color = 'red';
                    break;
            }
            break;
        case 'scissors':
            switch (computerSelection) {
                case 'Rock':
                    computerScore ++;
                    result.textContent = `You lose! Rock beats Scissors`;
                    result.style.color = 'red';
                    break;
                case 'Paper':
                    playerScore ++;
                    result.textContent = `You win! Scissors beat Paper`;
                    result.style.color = 'green';
                     break;
                case 'Scissors':
                    result.textContent = `Tie! You both chose Scissors!`;
                    result.style.color = 'black';
                    break;
            }
            break;
    }
    if (computerScore+playerScore>=5) {
        if (playerScore>computerScore) {
            score.textContent = `Well done, you won! 
                You won ${playerScore} round${(playerScore===1) ? '' : 's'}, 
                the computer won ${computerScore}`;
            score.style.color = 'green';
        } else {
            score.textContent = `You lost, better luck next time. 
                You won ${playerScore} round${(playerScore===1) ? '' : 's'}, 
                the computer won ${computerScore}`;
            score.style.color = 'red';
        }
    } else {
        updateScore();
    }
}

function startNewGame() {
    playerScore = 0;
    computerScore = 0;
    score.style.color = 'black';
    updateScore();
    result.textContent = '';
}

const playButtons = document.querySelectorAll('.play');
playButtons.forEach(button => button.addEventListener('click',playRound));

const newGame = document.querySelector('#newGame');
newGame.addEventListener('click', startNewGame);

const result = document.querySelector('#result');
const score = document.querySelector('#score');

let playerScore = 0;
let computerScore = 0;