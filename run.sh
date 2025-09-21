g++ -O3 -std=c++20 -Wall -Wextra -Wpedantic \
    main.cpp \
    -o runner \
    -lraylib -lm -ldl -lpthread -lGL -lX11
