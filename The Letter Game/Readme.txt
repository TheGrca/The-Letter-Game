The Letter Game using simple TCP. Here are the rules.

- Two clients participate in the game. In order for the game to start, it is necessary that both clients first
establish a connection with the server. 
- The letter to be played is set on the server. The server sends a notification to the clients about the start of the game and the letter to be played
- Each of the clients has the opportunity to choose his word on the given letter and sends it to the server.
- The server checks whether the incoming words start with the given letter
- The game continues until one client makes a mistake or gives up. The client quits the game by sending the word "End".
- If both clients make a mistake at the same time, the game continues.
- At the end of the game, the server sends the game result to the clients.
- If both clients give up at the same time, the winner is the one whose total sum of lengths sent
more correct words.