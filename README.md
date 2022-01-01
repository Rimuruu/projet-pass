# projet-pass

# Subject

Password game, two players try to guess each other words with a limited number of guess.
At the start of a round, The first player bet on the number of guess they can give then he choose the word he want the other to guess, lastly he give a hint on this word. The second player take a guess, if he doesn't guess the correct word then a another hint is given then a guess until they find. When the two players exceed the number of guess, the round end and their score is 0. When they find the word before exceeding the limited number of guess, the score is 11 minus the number of guess bet. Players change role at the start of the next round.
Word can't exceed 32 characters and we can bet at most 10 words. The number of round is limited to 5.

# Compiling

In the src folder, make or make all for compiling the server and the client. make debug add some debug information like packet information.

# Launch

Launching the server : ./server NumberOfRound ServerPort ServerIp
The number of round must be between 1 and 5. ServerIp is not mandatory, by default it is 127.0.0.1.

For the client : ./client ServerPort ServerIp

# Scenario

Start the server first, if the client is started first it will see the server is disconnect and will retry indefinitly to connect to the server. When the server is started, it will wait until 2 clients are connected in the lobby. A player can disconnect and reconnect. When they are 2 players the game start. The first client connected is player one, he input the
bet (if he type a incorrect integer by default the bet will be 10) then he input the word to guess and a hint (The hint word wan't be the word guess, only the 32 characters input will be taken even). Both player receive a word list of hint and guess then the player take a guess. If a player or the server disconnect the game end. If a third player try to connect during a game the server will refused him and advert him that the server is full.

## A classic game between two player

### Server

./server 2 7779 192.168.0.1
Server listening 192.168.0.15:7779
Waiting player to connect
One player connected
Number of player 1/2
One player connected
Number of player 2/2
Player bet on 3 words
Player 1 choose word : boat
Player 1 give the hint : sea
Player 2 guess : fish
Player 1 give the hint : sailling
Player 2 guess : boat
Player bet on 2 words
Player 1 choose word : fruit
Player 1 give the hint : food
Player 2 guess : vegetable
Player 1 give the hint : sweet
Player 2 guess : candy
Game over

### Client 1

./client 7779 192.168.0.15
Connecting to server 192.168.0.15:7779
Connected
Your are player 1

Take your bet (max 10) :
3
You bet on 3 words
Player 1 bet on 3 words

Type the word you want to be guessing (max 32 char) :
boat
You choose the word : boat
Give a hint to your partner (max 32 char) :
boat
You choose the word : boat
The hint can't be the guess

Give a hint to your partner (max 32 char) :
sea
You choose the word : sea
Word Hint Recap
Word 1 | sea

Word Guess Recap
Word 1 | fish

Give a hint to your partner (max 32 char) :
sailling
You choose the word : sailling
Word Hint Recap
Word 1 | sea
Word 2 | sailling

Word Guess Recap
Word 1 | fish
Word 2 | boat

You guessed the correct word.
Your score 8

Your are player 2

Player 1 bet on 2 words

Word Hint Recap  
Word 1 | food

Take a guess (max 32 char) :  
vegetable  
You choose the word : vegetable  
Word Guess Recap  
Word 1 | vegetable

Word Hint Recap  
Word 1 | food  
Word 2 | sweet

Take a guess (max 32 char) :  
candy  
You choose the word : candy  
Word Guess Recap  
Word 1 | vegetable  
Word 2 | candy

You have exhausted all you attempt.  
Your score 8

Game over  
 Your final score 8

Game over Server disconnected

### Client 2

./client 7779 192.168.0.15
Connecting to server 192.168.0.15:7779
Connected
Your are player 2

Player 1 bet on 3 words

Word Hint Recap
Word 1 | sea

Take a guess (max 32 char) :
fish
You choose the word : fish
Word Guess Recap
Word 1 | fish

Word Hint Recap
Word 1 | sea
Word 2 | sailling

Take a guess (max 32 char) :
boat
You choose the word : boat
Word Guess Recap
Word 1 | fish
Word 2 | boat

You guessed the correct word.
Your score 8

Your are player 1

Take your bet (max 10) :
2
You bet on 2 words
Player 1 bet on 2 words

Type the word you want to be guessing (max 32 char) :
fruit
You choose the word : fruit
Give a hint to your partner (max 32 char) :  
food  
You choose the word : food  
Word Hint Recap  
Word 1 | food

Word Guess Recap  
Word 1 | vegetable

Give a hint to your partner (max 32 char) :  
sweet  
You choose the word : sweet  
Word Hint Recap  
Word 1 | food  
Word 2 | sweet

Word Guess Recap  
Word 1 | vegetable  
Word 2 | candy

You have exhausted all you attempt.  
Your score 8

Game over  
 Your final score 8

Game over Server disconnected

## A player disconnect from lobby

### Server

./server 2 7778 192.168.0.15
Server listening 192.168.0.15:7778
Waiting player to connect
One player connected
Number of player 1/2
One player disconnected
Number of player 0/2
One player connected
Number of player 1/2

### Client 1

./client 7778 192.168.0.15
Connecting to server 192.168.0.15:7778
Connected
CTRL+C
./client 7778 192.168.0.15
Connecting to server 192.168.0.15:7778
Connected

## A third player try to join during a game

### Server

./server 2 7778 192.168.0.15
Server listening 192.168.0.15:7778
Waiting player to connect
One player connected
Number of player 1/2
One player connected
Number of player 2/2
Refused incomming client server full

### Client 1

./client 7778 192.168.0.15
Connecting to server 192.168.0.15:7778
Connected
Your are player 1

Take your bet (max 10) :

### Client 2

./client 7778 192.168.0.15
Connecting to server 192.168.0.15:7778
Connected
Your are player 2

### Client 3

./client 7778 192.168.0.15
Connecting to server 192.168.0.15:7778
Connected
Server full

Game over Server disconnected

## A player disconnect during a game

### Server

./server 2 7778 192.168.0.15
Server listening 192.168.0.15:7778
Waiting player to connect
One player connected
Number of player 1/2
One player connected
Number of player 2/2
Player disconnect

### Client 1

./client 7778 192.168.0.15
Connecting to server 192.168.0.15:7778
Connected
Your are player 1

Take your bet (max 10) :
CTRL+C

### Client 2

./client 7778 192.168.0.15
Connecting to server 192.168.0.15:7778
Connected
Your are player 2

Game over
The other player has disconnected

Game over Server disconnected

## The server disconnect during a game

### Server

./server 2 7778 192.168.0.15
Server listening 192.168.0.15:7778
Waiting player to connect
One player connected
Number of player 1/2
One player connected
Number of player 2/2
CTRL+C

### Client 1

./client 7778 192.168.0.15
Connecting to server 192.168.0.15:7778
Connected
Your are player 1

Take your bet (max 10) :
Game over Server disconnected

### Client 2

./client 7778 192.168.0.15
Connecting to server 192.168.0.15:7778
Connected
Your are player 2

Game over Server disconnecte
