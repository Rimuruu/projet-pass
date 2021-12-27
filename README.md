# projet-pass

Sujet :

Jeu de mot de passe, deux joueurs doivent se faire deviner un mot à tour de rûle.
Au début d'un round, le premier joueur parie sur un nombre maximum d'essais puis choisis le mot qui doit faire deviner et un premier indice sur le mot. Le second joueur essaye de deviner, s'il ne trouve pas le premier joueur donne un autre indice et ainsi de suite jusqu'à dépasser le nombre maximum parier au début. Si le joueur, le score est est calculé selon le partie de départ. Au round suivant, les roles s'échange.
Les mots ne peuvent pas dépassé 32 caractère et on peut parier au maximum jusqu'à 10 mots. Le nombre de round est limite à 5.

Compilation:
Dans le fichier src make pour compiler le client et le serveur (server). make debug rajoute des informations supplémentaires sur les erreurs et paquets envoyés.

Lancement:

Pour le serveur : ./server NombreDeRound PortDuServeur IpDuServeur
Le nombre de round du serveur doit être compris entre 1 et 5

Pour le client : ./client PortDuServeur IpDuServeur

Scénario :

For the different scenario, I did screenshot that show the diffent behaviour. They are in the same folder as

A classic game between two player

scenario1.png

        Server

./server 2 7779
Server listening 127.0.0.1:7779
Waiting player to connect
One player connected
Number of player 1/2
One player connected
Number of player 2/2
Player bet on 2 words
Player 1 choose word : boat
Player 1 give the hint : sea
Player 2 guess : boat
Player bet on 1 words
Player 1 choose word : hat
Player 1 give the hint : clothe
Player 2 guess : shoes

        Player 1

./client 7779 127.0.0.1
Connecting to server 127.0.0.1:7779
Your are player 1
Take your bet (max 10) :
2
You bet on 2 words
Player 1 bet on 2 words

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
Word 1 | boat
You guessed the correct word.
Your score 9
Your are player 2
Player 1 bet on 1 words
Word Hint Recap
Word 1 | clothe
Take a guess (max 32 char) :
shoes
You choose the word : shoes
Word Guess Recap
Word 1 | shoes
You have exhausted all you attempt.
Your score 9
Game over
Your final score 9
Game over Server disconnected

        Player 2

./client 7779 127.0.0.1
Connecting to server 127.0.0.1:7779
Your are player 2
Player 1 bet on 2 words
Word Hint Recap
Word 1 | sea
Take a guess (max 32 char) :
boat
You choose the word : boat
Word Guess Recap
Word 1 | boat
You guessed the correct word.
Your score 9
Your are player 1
Take your bet (max 10) :
1
You bet on 1 words
Player 1 bet on 1 words
Type the word you want to be guessing (max 32 char) :
hat
You choose the word : hat
Give a hint to your partner (max 32 char) :
clothe
You choose the word : clothe
Word Hint Recap
Word 1 | clothe
Word Guess Recap
Word 1 | shoes
You have exhausted all you attempt.
Your score 9
Game over
Your final score 9
Game over Server disconnected

A player disconnect from lobby :

        Server

./server 2 7777
Server listening 127.0.0.1:7777
Waiting player to connect
One player connected
Number of player 1/2
One player disconnected
Number of player 0/2
One player connected
Number of player 1/2

        Player 1

./client 7777 127.0.0.1
Connecting to server 127.0.0.1:7777
./client 7777 127.0.0.1
Connecting to server 127.0.0.1:7777

A third player try to join during a game :

A player disconnect during a game :

The server disconnect during a game :
