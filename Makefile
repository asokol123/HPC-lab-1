all: single v1 v2 dp

single: single.cpp
	g++ -Ofast -o single single.cpp -std=gnu++17

v1: v1.cpp
	g++ -Ofast -o v1 v1.cpp -std=gnu++17 -pthread

v2: v2.cpp
	g++ -Ofast -o v2 v2.cpp -std=gnu++17 -pthread

dp: dp.cpp
	g++ -Ofast -o dp dp.cpp -std=gnu++17

v1-tsan: v1.cpp
	g++ -Ofast -o v1 v1.cpp -std=gnu++17 -pthread -fsanitize=thread

v2-tsan: v2.cpp
	g++ -Ofast -o v2 v2.cpp -std=gnu++17 -pthread -fsanitize=thread

tsan: v1-tsan v2-tsan

clean:
	rm -f v1 v2 dp single
