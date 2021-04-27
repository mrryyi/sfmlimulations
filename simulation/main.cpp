#include <SFML/Graphics.hpp>
#include <vector>

const unsigned int window_width = 1000;
const unsigned int window_height = 1000;

const unsigned int pixels_width = 250;
const unsigned int pixels_height = 250;

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
unsigned int max_index_x = pixels_width - 1;
unsigned int max_index_y = pixels_height - 1;

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

double gen_new_value(pixel in_buffer[][pixels_height], int x, int y) {

    double neighbours[8];

    // above
    neighbours[0] = in_buffer[x][correct_y(y - 1)].value;
    // below
    neighbours[1] = in_buffer[x][correct_y(y + 1)].value;
    // left
    neighbours[2] = in_buffer[correct_x(x - 1)][y].value;
    // right
    neighbours[3] = in_buffer[correct_x(x + 1)][y].value;
    // up left
    neighbours[4] = in_buffer[correct_x(x - 1)][correct_y(y - 1)].value;
    // up right
    neighbours[5] = in_buffer[correct_x(x + 1)][correct_y(y - 1)].value;
    // down left
    neighbours[6] = in_buffer[correct_x(x - 1)][correct_y(y + 1)].value;
    // down right
    neighbours[7] = in_buffer[correct_x(x + 1)][correct_y(y + 1)].value;

    int living_neighbours = 0;
    for (int i = 0; i < 8; i++) {
        if (neighbours[i] > 0.5) {
            living_neighbours += 1;
        }
    }

    if (x == 100 & y == 101)
        printf("Living neighbours: %d\n", living_neighbours);
    
    double value;
    
    if (living_neighbours > 3 || living_neighbours < 2)
        return 0;
    if (living_neighbours == 3)
        return 1;


    return in_buffer[x][y].value;
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML works!");
    sf::View view(sf::Vector2f(pixels_width/2, pixels_height/2), sf::Vector2f(pixels_width, pixels_height));
    window.setView(view);

    
    unsigned int framelimit = 1;
    window.setFramerateLimit(3);
    
    sf::CircleShape shape(100.f);

    shape.setFillColor(sf::Color::Green);

    sf::Image buffer;
    sf::Sprite sprite;
    sf::Texture texture;

    pixel pixels_buf1[pixels_width][pixels_height];
    pixel pixels_buf2[pixels_width][pixels_height];

    for (int x = 0; x < pixels_width; x++) {
        for (int y = 0; y < pixels_height; y++) {
            pixels_buf1[x][y].x = x;
            pixels_buf1[x][y].y = y;
            pixels_buf1[x][y].value = ((double) rand()) / RAND_MAX;
        }
    }




    buffer.create(pixels_width, pixels_height, sf::Color(12, 12, 12));

    for (int x = 0; x < pixels_width; x++) {
        for (int y = 0; y < pixels_height; y++) {
            buffer.setPixel(x, y, sf::Color(x, y, y-x));
        }
    }

    texture.loadFromImage(buffer);
    sprite.setTexture(texture);

    ActiveBuffer active_buffer = buf1;

    while (window.isOpen())
    {

        window.setFramerateLimit(framelimit);
        framelimit++;
        if (framelimit > 30) {
            framelimit = 15;
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        


        double value;
        for (int x = 0; x < pixels_width; x++) {
            for (int y = 0; y < pixels_height; y++) {
                
                switch (active_buffer) {
                case buf1:
                    // buf2 new active buffer
                    pixels_buf2[x][y].value = gen_new_value(pixels_buf1, x, y);

                    break;
                case buf2:
                    // buf1 new active buffer
                    pixels_buf1[x][y].value = gen_new_value(pixels_buf2, x, y);

                    break;
                }
            }
        }

        if (active_buffer == buf1)
            active_buffer = buf2;
        else
            active_buffer = buf1;

        for (int x = 0; x < pixels_width; x++) {
            for (int y = 0; y < pixels_height; y++) {
                switch (active_buffer) {
                case buf1:
                    value = pixels_buf1[x][y].value;
                    break;
                case buf2:
                    value = pixels_buf2[x][y].value;
                    break;
                }
                buffer.setPixel(x, y, sf::Color(value * 255, value * 255, value * 255));
            }
        }


        texture.loadFromImage(buffer);

        sprite.setTexture(texture);



        window.draw(sprite);
        window.display();
    }

    return 0;
}