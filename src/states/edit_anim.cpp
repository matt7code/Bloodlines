#include "states/edit_anim.h"

Edit_Anim Edit_Anim::m_Edit_Anim;

void Edit_Anim::Init(Engine* engine) {
    e = engine;
    e->m_RenderWindow.setMouseCursorVisible(true);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.addBox("Edit_Anim", sf::Vector2f(0,0), sf::Vector2f(0,0), Box::BOX_NONE);
    m_line = &m_lineRed;
    m_interval = 0.03333333f;
    m_lastPos = sf::Vector2f(0,0);
}

void Edit_Anim::Cleanup(Engine* engine) {
    e->m_RenderWindow.setMouseCursorVisible(false);
    m_Boxes.getBoxes().clear();
}

void Edit_Anim::Resume(Engine* engine) {
    //std::cout << "EditTriggers Resume" << std::endl;
    if (m_mode == MODE_SAVE) {
        if(e->getInputString() == "CaNcElEd") {
            std::cout << "Save canceled." << std::endl;
        } else {
            std::ofstream of("anims/" + e->getInputString() + ".ani");
            if (!of) {
                std::cerr << "Error opening anims/" << e->getInputString() << ".ani\" for output." << std::endl;
            } else {
                for(auto point : (*m_line)) {
                    of << point.x << "\t" << point.y << std::endl;
                }
                std::cout << "Animation saved as \"anims/" << e->getInputString() << ".ani\"." << std::endl;
                AddToAnimDB(e->getInputString());
            }
        }
    }
    if (m_mode == MODE_LOAD) {
        if(e->getInputString() == "CaNcElEd") {
            std::cout << "Load canceled." << std::endl;
        } else {
            std::ifstream in("anims/" + e->getInputString() + ".ani");
            if (!in) {
                std::cerr << "Error opening \"anims/" << e->getInputString() << ".ani\" for input." << std::endl;
            } else {
                m_line->clear();
                float temp_x, temp_y;
                std::string input;
                std::stringstream ss;

                while(std::getline(in, input)) {
                    ss.clear();
                    ss.str(input);
                    ss >> temp_x >> temp_y;
                    m_line->push_back(sf::Vector2f(temp_x, temp_y));
                }
                std::cout << "Loaded \"anims/" << e->getInputString() << ".ani\"." << std::endl;
            }
        }
    }
    if (m_mode == MODE_FPS) {
        float fps = 30.0f;
        std::stringstream ss;
        ss << e->getInputString();
        ss >> fps;
        if (fps > 5.0f && fps < 120.0f) {
            m_interval = 1.0f / fps;
        } else {
            m_interval = 1.0f / 30.0f;
        }
    }
    m_mode = MODE_NONE;
}

void Edit_Anim::HandleEvents(Engine* engine) {
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            e->Quit();
        }

        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                e->PopState();
                break;
            case sf::Keyboard::S:
                m_mode = MODE_SAVE;
                e->setInputString("Enter a file name to save the animation in.:Existing Files:\n" + ComposeFileList());
                e->PushState(Input_String::Instance());
                break;
            case sf::Keyboard::L:
                m_mode = MODE_LOAD;
                e->setInputString("Enter an animation file name to load.:Existing Files:\n" + ComposeFileList());
                e->PushState(Input_String::Instance());
                break;
            case sf::Keyboard::F:
                m_mode = MODE_FPS;
                e->setInputString("Enter the desired FPS to record the animation at (15,30,60,90):");
                e->PushState(Input_String::Instance());
                break;

            case sf::Keyboard::R:
                m_lineRed.clear();
                m_lineYellow.clear();
                m_lineGreen.clear();
                m_lineBlue.clear();
                m_lineCyan.clear();
                m_lineMagenta.clear();
                m_lineWhite.clear();
                break;

            case sf::Keyboard::Num1:
                m_line = &m_lineRed;
                break;
            case sf::Keyboard::Num2:
                m_line = &m_lineYellow;
                break;
            case sf::Keyboard::Num3:
                m_line = &m_lineGreen;
                break;
            case sf::Keyboard::Num4:
                m_line = &m_lineBlue;
                break;
            case sf::Keyboard::Num5:
                m_line = &m_lineCyan;
                break;
            case sf::Keyboard::Num6:
                m_line = &m_lineMagenta;
                break;
            case sf::Keyboard::Num7:
                m_line = &m_lineWhite;
                break;

            case sf::Keyboard::G:
                m_grid++;
                if(m_grid == GRID_MAX) m_grid = GRID_OFF;
                break;

            default:
                break;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            m_line->clear();
            m_recording = true;
            m_lastPos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_ViewBase);
            m_line->push_back(m_lastPos);
            std::cout << "Initial m_lastPos: " << m_lastPos.x << ", " << m_lastPos.y << std::endl;
            m_recordStart = e->getTimeRunning().getElapsedTime();
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            m_recording = false;
            m_recordEnd = e->getTimeRunning().getElapsedTime();
        }
    }
}

void Edit_Anim::Update(Engine* engine) {
    if(m_recording) {
        float delta = e->getTimeRunning().getElapsedTime().asSeconds() - m_prevInterval;
        if(delta > m_interval) {
            sf::Vector2f mousePos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_ViewBase);
            m_prevInterval = e->getTimeRunning().getElapsedTime().asSeconds();
            m_line->push_back(mousePos - m_lastPos);
            m_lastPos = mousePos;
            std::cout << m_line->back().x << "\t" << m_line->back().y << std::endl;
        }
    }
}

void Edit_Anim::Draw(Engine* engine) {
    if(m_grid != GRID_OFF) DrawGrid();
    DrawHud();
    m_Boxes.DrawBoxes(e->m_RenderWindow);
    DrawLine(m_lineWhite);
    DrawLine(m_lineMagenta);
    DrawLine(m_lineCyan);
    DrawLine(m_lineBlue);
    DrawLine(m_lineGreen);
    DrawLine(m_lineYellow);
    DrawLine(m_lineRed);
}

void Edit_Anim::DrawGrid() {
    const int half_x = 1920 / 2;
    const int half_y = 1080 / 2;
    const int GRID_SIZE = 32*m_grid;

    for(int x = -half_x; x < half_x; x += GRID_SIZE) {
        for(int y = -half_y; y < half_y; y += GRID_SIZE) {
            sf::RectangleShape box(sf::Vector2f(GRID_SIZE,GRID_SIZE));
            box.setPosition(x + half_x, y + half_y);
            box.setOutlineColor(sf::Color(50,50,50));
            box.setOutlineThickness(1);
            box.setFillColor(sf::Color::Transparent);
            e->m_RenderWindow.draw(box);
        }
    }
}

void Edit_Anim::DrawLine(std::vector<sf::Vector2f>& line) {
    // Create a Dot
    sf::CircleShape dot(3);
    if(line == m_lineRed) dot.setFillColor(sf::Color::Red);
    else if(line == m_lineYellow) dot.setFillColor(sf::Color::Yellow);
    else if(line == m_lineGreen) dot.setFillColor(sf::Color::Green);
    else if(line == m_lineBlue) dot.setFillColor(sf::Color::Blue);
    else if(line == m_lineCyan) dot.setFillColor(sf::Color::Cyan);
    else if(line == m_lineMagenta) dot.setFillColor(sf::Color::Magenta);
    else if(line == m_lineWhite) dot.setFillColor(sf::Color::White);
    else dot.setFillColor(sf::Color::White);

    // Position it at the start of a line.
    for(auto it = line.begin(); it != line.end(); ++it) {
        if(it == line.begin()) {
            dot.setPosition(*it);
            //std::cout << "\n\n  Set: " << it->x << ", " << it->y << std::endl;
            e->m_RenderWindow.draw(dot);
        } else {
            dot.move(*it);
            //std::cout << " Move: " << it->x << ", " << it->y << std::endl;
            e->m_RenderWindow.draw(dot);
        }
    }
}

void Edit_Anim::DrawHud() {
    /// Green HUD
    sf::VertexArray line(sf::Lines, 2);
    line[0].color = sf::Color(0, 128, 0);
    line[1].color = sf::Color(0, 128, 0);

    line[0].position = sf::Vector2f(1920/2, 0);
    line[1].position = sf::Vector2f(1920/2, 1080);
    e->m_RenderWindow.draw(line);

    line[0].position = sf::Vector2f(0, 1080/2);
    line[1].position = sf::Vector2f(1920, 1080/2);
    e->m_RenderWindow.draw(line);

    /// Very dark blue
    sf::RectangleShape roomBorder(sf::Vector2f(1820,680));
    roomBorder.setPosition(50,50);
    roomBorder.setFillColor(sf::Color::Transparent);
    roomBorder.setOutlineColor(sf::Color(0, 0, 64));
    roomBorder.setOutlineThickness(1);
    e->m_RenderWindow.draw(roomBorder);

    /// Dark Yellow X's
    line[0].color = sf::Color(64, 64, 0);
    line[1].color = sf::Color(64, 64, 0);

    line[0].position = sf::Vector2f(426, 680);
    line[1].position = sf::Vector2f(480, 632);
    e->m_RenderWindow.draw(line);
    line[0].position = sf::Vector2f(426,632);
    line[1].position = sf::Vector2f(480,680);
    e->m_RenderWindow.draw(line);
    line[0].position = sf::Vector2f(1268,688);
    line[1].position = sf::Vector2f(1332,640);
    e->m_RenderWindow.draw(line);
    line[0].position = sf::Vector2f(1268,640);
    line[1].position = sf::Vector2f(1332,688);
    e->m_RenderWindow.draw(line);

    m_Boxes.getBox("Edit_Anim").getText().clear();
    std::stringstream ss;
    if(m_recording) {
        m_time = e->getTimeRunning().getElapsedTime().asSeconds() - m_recordStart.asSeconds();
    }
    ss << "\nG: Grid " << m_grid << " of " << GRID_MAX - 1 << "     Time Recording: " << m_time << std::endl << std::endl;
    ss << "L: Load     S: Save     R: Reset" << std::endl;
    ss << "1: Red    2: Yellow    3: Green    4: Blue    5: Cyan    6: Magenta    7: White";

    m_Boxes.getBox("Edit_Anim").addText(ss.str(), sf::Vector2f(50,900));
    m_Boxes.getBox("Edit_Anim").addText("Monsters", sf::Vector2f(458-48,676));
    m_Boxes.getBox("Edit_Anim").addText("Party", sf::Vector2f(1300-30,684));
}

void Edit_Anim::AddToAnimDB(std::string file_name)
{
    std::set<std::string> AnimFileNames;
    std::ifstream in("anims/anims.db");
    std::string input;
    while(std::getline(in, input)) {
        AnimFileNames.insert(input);
    }
    in.close();
    AnimFileNames.insert(file_name);
    std::ofstream of("anims/anims.db");
    for(const auto s : AnimFileNames) of << s << std::endl;
    of.close();
}

std::string Edit_Anim::ComposeFileList()
{
    std::set<std::string> AnimFileNames;
    std::ifstream in("anims/anims.db");
    std::string input;
    while(std::getline(in, input)) {
        AnimFileNames.insert(input);
    }
    in.close();
    std::stringstream ss;
    int i = 0;
    for(const auto s : AnimFileNames) {
        if(!s.empty()) {
            std::cout << "AnimFile #" << i << std::endl;
            ss << Tool::padLeft(s, 25);
            if(++i % 4 == 0) ss << "\n";
        }
    }
    return ss.str();
}
