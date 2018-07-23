//
// Created by Lionel Aimerie  on 17/7/18.
//

#include "App.h"

//#include <SFML/Config.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <SFML/Audio.hpp>

//#include "sfmlUtils.hpp"

// activity for paths
#include <android/api-level.h>
#include <android/native_activity.h>
#include <SFML/System/NativeActivity.hpp>

#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>

#include <GLES2/gl2.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity", __VA_ARGS__))


namespace mvd
{
  //=== CONSTRUCT (private)
  App::App()
    : m_paused(false)
  {
    // random seed f(time)
    srand(static_cast<unsigned int>(time(nullptr)));
    LOGI("APP constructor with random seed %d", rand() % 512);
  }



  void App::fiddleWithPaths() {
    // fiddle with paths
    ANativeActivity* na = sf::getNativeActivity();
    std::string supportPath = na->internalDataPath;
    std::string spath = supportPath + + "/someFileName.txt";
    const char *path = spath.c_str();

    // 1 st try to read
    std::ifstream si(path);
    std::string buf = "Init";
    if (si.good()) {
      si >> buf;
      si.close();
    }

    LOGI("read=[%s]", buf.c_str());
    if (buf=="Hello") {
      buf = "ByeBye";
    } else {
      buf = "Hello";
    }

    LOGI("Writing to: [%s]", path);
    std::ofstream stream(path, std::ios_base::out | std::ios_base::trunc);
    try {

      // stream.open();

      if(stream.good()) {
        stream << buf;
        // add a time tag
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        stream << " (finished computation at " << std::ctime(&end_time) << ")";
        LOGI("Stream is good");

        stream.close();
      }
    }
    catch (std::ios_base::failure& e) {
      LOGI("Error no: %s", strerror(errno));
    }

  }



  //--- LOOP
  // Application main loop
  // main loop
  // FPS sync based one VBL, done by openGL through SFML::Window
  void App::loop()
  {
    LOGI("entered App.loop");

    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
    for (std::size_t i = 0; i < modes.size(); ++i) {
      sf::VideoMode mode = modes[i];
      LOGI(" mode [%d]=(w:%d, h:%d) bpp=%d", i, mode.width, mode.height, mode.bitsPerPixel);
    }
// Create a window with the same pixel depth as the desktop
    //sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    //sf::RenderWindow window(sf::VideoMode(1024, 768, desktop.bitsPerPixel), "SFML window");

    // something goes worng in the RenderWindow creation
    sf::VideoMode vm = sf::VideoMode::getDesktopMode();
    LOGI("window creating...");
    sf::RenderWindow window(vm, "");
    LOGI("window created");
    sf::View view = window.getDefaultView();

    sf::Vector2f wxy(view.getSize());
    sf::FloatRect rvp(view.getViewport());
    LOGI("window(%4.2f,%4.2f) viewport(%4.2f,%4.2f,%4.2f,%4.2f)", wxy.x, wxy.y, rvp.left, rvp.top, rvp.width,rvp.height);

    // try Window::getSettings()

    // load the 2 textures
    sf::Texture texture_bg, texture_fg;
    if(!texture_bg.loadFromFile("gradients_bg.png")) {
      LOGE("couldn't find file [gradients_bg]");
      return;
    } else {
      LOGI("found file [gradients_bg]");
    }
    if(!texture_fg.loadFromFile("gradients.png")) {
      LOGE("couldn't find file [gradients]");
      return;
    }
    sf::Sprite spr_bg(texture_bg, sf::IntRect(0, 0, 400, 400));
    sf::Sprite spr_fg(texture_fg, sf::IntRect(0, 0, 400, 400));
    spr_bg.setPosition(0.f, 0.f);
    spr_fg.setPosition(0.f, 0.f);

    sf::Color cbg(0x80a0f0ff);

    // create render texture
    LOGI("rendering texture");
    sf::RenderTexture rt;
    rt.create(400, 400); //rt.create(400, 400, sf::ContextSettings)
    rt.setSmooth(false);

    //BlendMode (Factor colorSourceFactor, Factor colorDestinationFactor
    //    , Equation colorBlendEquation
    //  , Factor alphaSourceFactor, Factor alphaDestinationFactor
    //    , Equation alphaBlendEquation)

    // premultiplied
    /*
    sf::RenderStates rtst(sf::BlendMode(
      sf::BlendMode::SrcAlpha, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add,
      sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add
    ));
     */
    // white if not pre-multiplied, otherwise what i want
    // result WHITE, so... not premultiplied
    /*
    sf::RenderStates rtst(sf::BlendMode(
      sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add
    ));
    */
    // gray halo when premultiplied, otherwise what i wnat
    sf::RenderStates rtst;
    rtst.blendMode = sf::BlendAlpha;
    rtst.texture = nullptr;
    rtst.shader = nullptr;
    rtst.transform = sf::Transform::Identity;

    //LOGI("rtst.shader=%p", rtst.shader);
    rt.clear(); //sf::Color(0xff0000FF));
    rt.draw(spr_bg, rtst);
    //rt.display();
    rt.draw(spr_fg, rtst);
    rt.display();
    LOGI("rendering texture - done");


    sf::Vector2f xy(0.5f * wxy);
    if (wxy.x > wxy.y) {
      xy.y = 0.f;
    } else {
      xy.x = 0.f;
    }
    LOGI("center for rendered sprite (%4.1f,%4.1f)", xy.x, xy.y);

    sf::Sprite sprite(rt.getTexture());
    sprite.setPosition(xy);

    // check https://www.andersriggelsen.dk/glblendfunc.php
    //    bg-img: https://i.stack.imgur.com/XjLNW.png
    //    fg-img: https://i.stack.imgur.com/9CN6w.png
    // https://gamedev.stackexchange.com/questions/82741/normal-blend-mode-with-opengl-trouble

    // colorSourceFactor, colorDestinationFactor, colorBlendEquation,
    // alphaSourceFactor, alphaDestinationFactor, alphaBlendEquation
    /// premultiplied
    //sf::RenderStates rtst(sf::BlendMode(sf::BlendMode::SrcAlpha, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add, sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add));
    // not-premultiplied

    //sf::BlendMode bm(sf::BlendMode::SrcAlpha, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add, sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::Add);
    //sf::BlendMode bm(sf::BlendMode::SrcAlpha, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add, sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::Add);

    // test sound
    sf::SoundBuffer buffer;
    // loadFromFile has to be shielded like Image
    //bool isBufferOk = buffer.loadFromFile("canary.wav");
    bool isBufferOk = buffer.loadFromFile("arrow_hit.flac");
    if (isBufferOk) {
      LOGI("SoundBuffer loaded: time[%4.2f]", buffer.getDuration().asSeconds());
    }
    LOGI("buffer loaded? = [%s]", isBufferOk ? "yes" : "NO!");

    sf::Sound sound;
    sound.setBuffer(buffer);

    bool active = true;
    while (window.isOpen()) {
      sf::Event event;

      // handle events
      while (active ? window.pollEvent(event) : window.waitEvent(event)) {
        switch (event.type) {
          case sf::Event::Closed:
            window.close();
            break;
          case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape)
              window.close();
            break;
          case sf::Event::Resized:
            view.setSize(event.size.width, event.size.height);
            view.setCenter(event.size.width/2, event.size.height/2);
            window.setView(view);
            break;
          case sf::Event::LostFocus: break;
          case sf::Event::GainedFocus: break;
          // On Android MouseLeft/MouseEntered are (for now) triggered,
          // whenever the app loses or gains focus.
          case sf::Event::MouseLeft:
            active = false;
            break;
          case sf::Event::MouseEntered:
            active = true;
            break;
          case sf::Event::TouchBegan:
            if (event.touch.finger == 0) {
              //sound.stop();

              sound.play();
              LOGI("[TSOING] vol=[%2.1f] buffer=[%p]", sound.getVolume(), sound.getBuffer());
              LOGI("  loop=[%s]", sound.getLoop() ? "T" : "F");
              sf::Vector3f xyz(sound.getPosition());
              LOGI("  pos=[%f,%f,%f]", xyz.x, xyz.y, xyz.z);
              LOGI("  offset=[%f]", sound.getPlayingOffset().asSeconds());

            }
            break;
        }
      }

      if (active) {
        window.clear(cbg);
        window.draw(spr_bg);
        window.draw(spr_fg);

        window.draw(sprite, rtst);
        window.display();
      }
      else {
        sf::sleep(sf::milliseconds(100));
      }
    }
  }





}//eo-namespace mvd
