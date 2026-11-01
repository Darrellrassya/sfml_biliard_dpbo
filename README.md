- **Darrell Rassya Islamy** (`Game`, `Cue`)
- **Naufal Aqil Nasrullah** (`Ball`, `Table`)
- **Raihan Fathan Rayyan** (`ScoreBoard`, `Menu`)

terminal for mac :
rm -f app && g++ -std=c++17 src/*.cpp -Iinclude -I/usr/local/include -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system -o app && ./app