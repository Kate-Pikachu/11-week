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
private:
    float Frame, speed, time;
    Sprite sprite;
    std::vector<IntRect> frames;

public:
    Animation() {}

    Sprite* get_sprite() {
        return &sprite;
    }

    explicit Animation(Texture& t, int x, int y, int w, int h, int count, float Speed)
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

        int m = static_cast<int>(Frame);
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


    explicit Entity(Animation& a, int X, int Y, float Angle = 0, int radius = 1, bool Life = 1, double dx_ = 0, double dy_ = 0, Name name = Name::none)
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


    virtual void update() = 0;

    void death(bool Life) {
        life = Life;
    }

    Animation* get_anim() { return &anim_; }

    Name name() { return name_; }

    friend const bool isCollide(Entity* a, Entity* b);

    void draw(RenderWindow& app)
    {
        anim_.get_sprite() -> setPosition(x, y);
        anim_.get_sprite() -> setRotation(angle + 90);
        app.draw(*anim_.get_sprite());
    }

    virtual ~Entity() = default;
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
    explicit asteroid(Animation& a, int X, int Y, float Angle = 0, int radius = 1, bool Life = 1) :
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
        x += static_cast<int>(dx);
        y += static_cast<int>(dy);

        if (x > W) x = 0;  
        if (x < 0) x = W;
        if (y > H) y = 0;  
        if (y < 0) y = H;
    }

};


class bullet : public Entity
{
public:
    explicit bullet(Animation& a, int X, int Y, float Angle = 0, int radius = 1, bool Life = 1) :
        Entity(a, X, Y, Angle, radius, Life) {      

        name_ = Name::bullet;
    }

    void update() override
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
            dx *= num;
            dy *= num;
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
    Text text_points;

    srand(time(0));

    RenderWindow app(VideoMode(W, H), "Asteroids!");
    app.setFramerateLimit(60);

    Texture t1, t2, t3, t4, t5, t6, t7, t8, t9;

    t1.loadFromFile("...\\images\\spaceship.png");
    t2.loadFromFile("..\\images\\background.jpg");
    t3.loadFromFile("..\\images\\explosions/type_C.png");
    t4.loadFromFile("..\\images\\rock.png");
    t5.loadFromFile("..\\images\\fire_blue.png");
    t6.loadFromFile("..\\images\\rock_small.png");
    t7.loadFromFile("..\\images\\explosions\\type_B.png");
    t8.loadFromFile("..\\images\\health_points.png");
    t9.loadFromFile("..\\images\\game_over.png");

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
    //Animation background(t2);
    Animation health_points(t8, 0, 0, 225, 225, 1, 0);
    //Animation game_over(t9);
    app.setFramerateLimit(60);



    text_points.setFillColor(Color(255, 0, 0, 200));
    text_points.setOutlineColor(Color::Black);
    text_points.setOutlineThickness(2);
    text_points.setCharacterSize(40);
    text_points.setStyle(Text::Regular);
    text_points.setString(std::to_string(0));
    text_points.setPosition(20, 0);
    app.setFramerateLimit(60);

    std::list<std::shared_ptr<Entity>> entities;

    for (int i = 0; i < 15; i++)
    {

        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_int_distribution<int> ran_w(0, W);
        std::uniform_int_distribution<int> ran_h(0, H);
        std::uniform_int_distribution<int> ran(0, 360);

        std::shared_ptr<asteroid> a(new asteroid(sRock, ran_w(eng), ran_h(eng), ran(eng), 25));
        entities.push_back(a);
    }

    std::shared_ptr<player> p(new player(sPlayer, W / 2, H / 2, 0, 20));
    
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
                    std::shared_ptr<bullet> b(new bullet(sBullet, p->x, p->y, p->angle, 10));                    entities.push_back(b);
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
                        text_points.setString(std::to_string(p->get_points()));

                        std::shared_ptr<bullet> e(new bullet(sExplosion, a->x, a->y));                        entities.push_back(e);
                        entities.push_back(e);


                        if (a->R == 25) {
                            for (int i = 0; i < 2; i++)
                            {
                                std::random_device rd;
                                std::mt19937 eng(rd());
                                std::uniform_int_distribution<int> ran(0, 360);

                                std::shared_ptr<Entity> e(new asteroid(sRock_small, a->x, a->y, ran(eng), 15));
                                entities.push_front(e);
                            }
                        }

                    }

                if (a -> name() == Name::player && b -> name() == Name::asteroid)
                    if (isCollide(a, b)) {
                        b -> death(false);

                        std::shared_ptr<player> k(new player(sExplosion_ship, a->x, a->y));
                        entities.push_front(k);

                        if (p -> get_hp() > 1)
                        {
                            p = new player(sPlayer, W / 2, H / 2, 0, 20);
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
            std::shared_ptr<asteroid> a(new asteroid(sRock, randomint(0, W), randomint(0, H), randomint(0, 360), 25));
            entities.push_back(a);
        }

        for (auto i = entities.begin(); i != entities.end();)
        {
            std::shared_ptr<Entity> e = *i;

            e -> update();
            e -> get_anim()->update();;

            if (e->life == false) {
                i = entities.erase(i);
            }
            else i++;
        }

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
