#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <random>

using namespace sf;

const int W = 1200;
const int H = 800;

float DEGTORAD = 0.017453f;

enum class Name {
    asteroid,
    player,
    bullet,
    explosion,
    none
};


int randomint(int min, int max) {
    std::random_device rd;
    std::mt19937 en(rd());
    if (max < min)
        std::swap(max, min);
    std::uniform_int_distribution<int> ran(min, max);
    return ran(en);
}


class Animation
{
public:
    float Frame, speed, time;
    Sprite sprite;
    std::vector<IntRect> frames;

    Animation() {}

    Sprite* get_sprite() {
        return &sprite;
    }

    Animation(Texture& t, int x, int y, int w, int h, int count, float Speed)
    {
        Frame = 0;
        time = 0;
        speed = Speed;

        for (int i = 0; i < count; i++)
            frames.push_back(IntRect(x + i * w, y, w, h));

        sprite.setTexture(t);
        sprite.setOrigin(w / 2, h / 2);
        sprite.setTextureRect(frames[0]);
    }


    void update()
    {
        Frame += speed;
        time += speed;

        int n = frames.size();
        if (Frame >= n) Frame -= n;

        int m = std::trunc(Frame);
        if (n > 0) sprite.setTextureRect(frames[m]);
    }

    bool isEnd()
    {
        return time >= frames.size();;
    }

};


class Entity
{
public:

    float x, y, dx, dy, R = 1, angle = 0;
    bool life;
    Name name_;
    Animation anim_;


    Entity(Animation& a, int X, int Y, float Angle = 0, int radius = 1, bool Life = 1, double dx_ = 0, double dy_ = 0, Name name = Name::none)
    {
        anim_ = a;
        x = X; y = Y;
        angle = Angle;
        R = radius;
        life = Life;
        dx = dx_;
        dy = dy_;
        name_ = name;
    }

    Entity(Animation& a, int X, int Y, Name name, float Angle = 0, int radius = 1, bool Life = 1, double dx_ = 0, double dy_ = 0)
    {
        anim_ = a;
        x = X; y = Y;
        angle = Angle;
        R = radius;
        life = Life;
        dx = dx_;
        dy = dy_;
        name_ = name;

    }


    virtual void update() {};

    void death(bool Life) {
        life = Life;
    }

    const bool get_life() { return life; }

    const int get_r() { return R; }

    const int get_x() { return x; }

    const int get_y() { return y; }

    Animation* get_anim() { return &anim_; }

    Name name() { return name_; }

    friend const bool isCollide(Entity* a, Entity* b);

    void draw(RenderWindow& app)
    {
        anim_.get_sprite() -> setPosition(x, y);
        anim_.get_sprite() -> setRotation(angle + 90);
        app.draw(*anim_.get_sprite());
    }

    virtual ~Entity() {};
};


const bool isCollide(Entity* a, Entity* b)
{
    return (b->x - a->x) * (b->x - a->x) +
        (b->y - a->y) * (b->y - a->y) <
        (a->R + b->R) * (a->R + b->R);
}


class asteroid : public Entity
{
public:
    asteroid(Animation& a, int X, int Y, float Angle = 0, int radius = 1, bool Life = 1) :
        Entity(a, X, Y, Angle, radius, Life) {

        std::default_random_engine dre;
        dre.seed(time(0));
        std::uniform_int_distribution<int> uid(-4, 4);

        dx = uid(dre);
        dy = uid(dre);
        name_ = Name::asteroid;
    }

    void update() override
    {
        x += trunc(dx);
        y += trunc(dy);

        if (x > W) x = 0;  
        if (x < 0) x = W;
        if (y > H) y = 0;  
        if (y < 0) y = H;
    }

};


class bullet : public Entity
{
public:
    bullet(Animation& a, int X, int Y, float Angle = 0, int radius = 1, bool Life = 1) :
        Entity(a, X, Y, Angle, radius, Life) {      

        name_ = Name::bullet;
    }

    void  update() override
    {
        dx = cos(angle * DEGTORAD) * 6;
        dy = sin(angle * DEGTORAD) * 6;
        x += trunc(dx);
        y += trunc(dy);

        if (x > W || x < 0 || y > H || y < 0) life = 0;
    }

};


class player : public Entity
{
private:
    bool thrust_ = false;
    const double maxSpeed = 15;
    const double num = 0.99;
    int health_points = 5;
    int points = 0;

public:


    const double get_angle() { return angle; };

    void c_angle(double Angle) { angle += Angle; }
    void c_thrust(bool thrust) { thrust_ = thrust; }
    bool thrust() { return thrust_; }
    void anim(Animation& a) { anim_ = a; }
    void inc_points() { points++; }
    int get_points() { return points; }
    int get_hp() { return health_points; }
    void set_dx() { dx = 0; }
    void set_dy() { dy = 0; }
    void dec_hp() { health_points--; }

    player(Animation& a, int X, int Y, float Angle = 0, int radius = 1, bool Life = 1) :
        Entity(a, X, Y, Angle, radius, Life)
    {
        name_ = Name::player;
    }

    void settings(Animation& a, int X, int Y, float Angle = 0, int radius = 1, double dx_ = 0, double dy_ = 0)
    {
        anim_ = a;
        x = X; y = Y;
        angle = Angle;
        R = radius;
        dx = dx_;
        dy = dy_;
    }

    void update() override
    {
        if (thrust_)
        {
            dx += cos(angle * DEGTORAD) * 0.2;
            dy += sin(angle * DEGTORAD) * 0.2;
        }
        else
        {
            dx *= 0.99;
            dy *= 0.99;
        }

        float speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed)
        {
            dx *= maxSpeed / speed;
            dy *= maxSpeed / speed;
        }

        x += trunc(dx);
        y += trunc(dy);

        if (x > W) x = 0; 
        if (x < 0) x = W;
        if (y > H) y = 0; 
        if (y < 0) y = H;
    }

};



void init(std::string name, Texture T) {
    T.loadFromFile(name);
}


class Textures {
private:
    Texture texture;
public:
    explicit Textures(std::string name) {
        texture.loadFromFile(name);
    }
    Texture* textures() {
        return &texture;
    }

};


class Health_Points {
public:

    Animation anim;
    float x, y, angle;

    Health_Points(Animation& a, int X, int Y, float Angle = 0)
    {
        anim = a;
        x = X; y = Y;
        angle = Angle;

    }

    void draw(RenderWindow& app)
    {
        anim.get_sprite()->setPosition(x, y);
        anim.get_sprite()->setRotation(angle + 90);
        app.draw(*anim.get_sprite());
    }
};



int main()
{

    Sprite game_over;
    sf::Font font;
    Text text_points;

    srand(time(0));

    RenderWindow app(VideoMode(W, H), "Asteroids!");
    app.setFramerateLimit(60);

    Texture t1, t2, t3, t4, t5, t6, t7, t8, t9;

    t1.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/spaceship.png");
    t2.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/background.jpg");
    t3.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/explosions/type_C.png");
    t4.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/rock.png");
    t5.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/fire_blue.png");
    t6.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/rock_small.png");
    t7.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/explosions/type_B.png");
    t8.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/health_points.png");
    t9.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/game_over.png");

    t1.setSmooth(true);
    t2.setSmooth(true);

    Sprite background(t2);

    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
    Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);
   // Animation background(t2);
    Animation health_points(t8, 0, 0, 225, 225, 1, 0);
    //Animation game_over(t9);
    app.setFramerateLimit(60);



    font.loadFromFile("D:/4 ���/������/11 �������/Project/Debug/images/font.h");
    text_points.setFont(font);
    text_points.setFillColor(sf::Color(255, 0, 0, 200));
    text_points.setOutlineColor(sf::Color::Black);
    text_points.setOutlineThickness(2);
    text_points.setCharacterSize(40);
    text_points.setStyle(sf::Text::Regular);
    text_points.setString(std::to_string(0));
    text_points.setPosition(20, 0);
    app.setFramerateLimit(60);

    std::list<Entity*> entities;

    for (int i = 0; i < 15; i++)
    {
        asteroid* a = new asteroid(sRock, randomint(0, W), randomint(0, H), randomint(0, 360), 25);
        entities.push_back(a);
    }

    player* p = new player(sPlayer, W / 2, H / 2, 0, 20);
    entities.push_back(p);


    while (app.isOpen())
    {
        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Space)
                {
                    bullet* b = new bullet(sBullet, p -> get_x(), p -> get_y(), p -> get_angle(), 10);
                    entities.push_back(b);
                }

        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) p -> c_angle(3);
        if (Keyboard::isKeyPressed(Keyboard::Left))  p -> c_angle(-3);
        if (Keyboard::isKeyPressed(Keyboard::Up)) p -> c_thrust(true);
        else p->c_thrust(false);


        for (auto a : entities)
            for (auto b : entities)
            {
                if (a -> name() == Name::asteroid && b -> name() == Name::bullet)
                    if (isCollide(a, b))
                    {
                        a -> death(false);
                        b -> death(false);
                        p -> inc_points();

                        Entity* e = new Entity(sExplosion, a->get_x(), a->get_y(), Name::explosion);
                        entities.push_back(e);


                        if (a->get_r() == 25) {
                            for (int i = 0; i < 2; i++)
                            {
                                Entity* e = new asteroid(sRock_small, a->get_x(), a->get_y(), randomint(0, 360), 15);
                                entities.push_front(e);
                            }
                        }

                    }

                if (a -> name() == Name::player && b -> name() == Name::asteroid)
                    if (isCollide(a, b)) {
                        b -> death(false);

                        Entity* k = new Entity(sExplosion_ship, a->get_x(), a->get_y(), Name::explosion);
                        entities.push_back(k);

                        if (p -> get_hp() > 1)
                        {
                            p -> settings(sPlayer, W / 2, H / 2, 0, 20);
                            p -> set_dx(); 
                            p -> set_dy();
                            p -> dec_hp();

                        }
                        else
                        {
                            p->dec_hp();
                            for (int i = 0; i < 64; i++) {
                                app.draw(background);
                                k->get_anim()->update();
                                k->draw(app);
                                app.display();
                            }
                            app.draw(background);
                            app.display();
                        }
                    }
            }


        if (p -> get_hp() == 0) 
        {
            app.clear();
            app.draw(background);
            for (int i = 0; i < 255; i = i + 7) {

                game_over.setColor(Color(255, 255, 255, i));
                app.draw(game_over);
                app.display();
            }
        }


        if (p -> thrust())  p -> anim(sPlayer_go);
        else p -> anim(sPlayer);


        for (auto e : entities)
            if (e -> name() == Name::explosion)
                if (e->get_anim()->isEnd()) {
                    e->death(false);
                }

        if (randomint(0, 150) == 0)
        {
            asteroid* a = new asteroid(sRock, randomint(0, W), randomint(0, H), randomint(0, 360), 25);
            entities.push_back(a);
        }

        for (auto i = entities.begin(); i != entities.end();)
        {
            Entity* e = *i;

            e -> update();
            e -> get_anim()->update();;

            if (e -> get_life() == false) { 
                i = entities.erase(i); 
                delete e; }
            else i++;
        }

        //////draw//////
        Health_Points* hp;


        app.draw(background);
        for (auto i : entities) i->draw(app);

        for (auto i = 0; i < p -> get_hp(); i++) {
            hp = new Health_Points(health_points, 1050 + i * 75, 120, -90);
            hp -> draw(app);

        }
        app.draw(text_points);
        app.display();
    }


    system("pause");
    return EXIT_SUCCESS;
}
