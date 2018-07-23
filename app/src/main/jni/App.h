//
// Created by Lionel Aimerie  on 17/7/18.
//

#ifndef ANDROID_MODIFIED_APP_H
#define ANDROID_MODIFIED_APP_H

namespace mvd {
  // APP: [SINGLETON]
  // top level pseudo static class, instantiated only once
  // call with mvd::App::getInstance()->loop();
  class App
  {
  public:
    //--- METHODS

    void loop(); // main loop
    void fiddleWithPaths(); // test paths

    // get reference to unique instance and handle clean up gracefully
    static App& getInstance() {
      static App instance;
      return instance;
    };

    // deleted methods to avoid appearance of accidental copies of your singleton
    App(App const&) = delete; // Singleton: Don't Implement
    void operator=(App const&) = delete; // Singleton: Don't Implement



  private:
    //-- METHODS
    App(); // Singleton constructor

    //-- MEMBERS
    //unique_ptr<sf::RenderWindow> m_upWindow;
    bool m_paused;
  };

}//eo-namespace mvd
#endif //ANDROID_MODIFIED_APP_H
