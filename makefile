build: main.cpp
	g++ main.cpp -lsfml-graphics -lsfml-window -lsfml-system -lpthread -o main.exe

build-PE: pathEditor.cpp
	g++ pathEditor.cpp -lsfml-graphics -lsfml-window -lsfml-system -o pathEditor.exe

trainer:
	g++ trainer.cpp -lpthread -o trainer.exe

clear:
	rm *.exe

clean:
	rm DataSet/dataSet.bin meta-DataSet/dataSet.bin *.exe NeuralNets/*bin

run:
	./main.exe

path:
	./pathEditor.exe

train:
	./trainer.exe