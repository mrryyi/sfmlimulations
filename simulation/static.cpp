#include <SFML/Graphics.hpp>
#include <vector>

const unsigned int window_height = 500;
const unsigned int window_width = 500;

struct pixel {
    int x;
    int y;
    double value;
};

/*
Rules
    step 1 pixels value is between 0 and 1
    step 2 pixels value adds the average of neighbouring pixels
    step 3 pixels value decreases by some percentage each tick
*/
unsigned int max_index_x = window_width - 1;
unsigned int max_index_y = window_height - 1;

int correct_x(int x) {
    if (x < 0)
        return max_index_x;
    if (x > max_index_x)
        return 0;
    return x;
}

int correct_y(int y) {
    if (y < 0)
        return max_index_y;
    if (y > max_index_y)
        return 0;
    return y;

}

enum ActiveBuffer {
    buf1,
    buf2
};

/*
Rules
    step 1 pixels value is between 0 and 1
    step 2 pixels value adds the average of neighbouring pixels divided by 2
    step 3 pixels value decreases by some percentage each tick
    
    if resulting value is above 1, reset to 0.
*/

double gen_new_value(pixel in_buffer[][window_height], int x, int y) {

    double value_above;
    double value_below;
    double value_left;
    double value_right;

    double value_upleft;
    double value_upright;
    double value_downleft;
    double value_downright;
    

    // VERTICAL
    value_above = in_buffer[x][correct_y(y - 1)].value;
    value_below = in_buffer[x][correct_y(y + 1)].value;
    // HORIZONTAL
    value_left = in_buffer[correct_x(x - 1)][y].value;
    value_right = in_buffer[correct_x(x + 1)][y].value;
    // DIAGONAL UP
    value_upleft = in_buffer[correct_x(x - 1)][correct_y(y - 1)].value;
    value_upright = in_buffer[correct_x(x + 1)][correct_y(y - 1)].value;
    // DIAGONAL DOWN
    value_downleft = in_buffer[correct_x(x - 1)][correct_y(y + 1)].value;
    value_downright = in_buffer[correct_x(x + 1)][correct_y(y + 1)].value;

    double average_of_all = (value_above + value_below
                           + value_left + value_right
                           + value_upleft + value_upright
                           + value_downleft + value_downright) / 8;

    

    double value = in_buffer[x][y].value + average_of_all;
    
    
    if (value > 1)
        value = 0;

    value *= 0.8;

    return value;
}

double return_same_value_times_two(pixel in_buffer[][window_height], int x, int y) {
    return in_buffer[x][y].value * 2;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Image buffer;
    sf::Sprite sprite;
    sf::Texture texture;

    pixel pixels_buf1[window_width][window_height];
    pixel pixels_buf2[window_width][window_height];

    for (int x = 0; x < window_width; x++) {
        for (int y = 0; y < window_height; y++) {
            pixels_buf1[x][y].x = x;
            pixels_buf1[x][y].y = y;
            pixels_buf1[x][y].value = 0;
            if (x == 100 && y == 100) {
                pixels_buf1[x][y].value = 2;
            }
        }
    }

    buffer.create(window_height, window_width, sf::Color(12, 12, 12));

    for (int x = 0; x < window_width; x++) {
        for (int y = 0; y < window_height; y++) {
            buffer.setPixel(x, y, sf::Color(x, y, y-x));
        }
    }

    texture.loadFromImage(buffer);
    sprite.setTexture(texture);

    ActiveBuffer active_buffer = buf1;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        


        double value;
        for (int x = 0; x < window_width; x++) {
            for (int y = 0; y < window_height; y++) {
                
                switch (active_buffer) {
                case buf1:
                    value = gen_new_value(pixels_buf1, x, y);
                    // buf2 new active buffer
                    pixels_buf2[x][y].value = value;

                    break;
                case buf2:
                    value = gen_new_value(pixels_buf1, x, y);
                    // buf1 new active buffer
                    pixels_buf1[x][y].value = value;

                    break;
                }

                buffer.setPixel(x, y, sf::Color(value * 255, value * 255, value * 255));
            }
        }

        if (active_buffer == buf1)
            active_buffer = buf2;
        else
            active_buffer = buf1;

        texture.loadFromImage(buffer);

        sprite.setTexture(texture);


        window.draw(sprite);
        window.display();
    }

    return 0;
}