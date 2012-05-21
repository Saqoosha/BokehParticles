#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
  ofAppGlutWindow window;
  ofSetupOpenGL(&window, 1024,768, OF_WINDOW);
//  ofSetupOpenGL(&window, 1024,768, OF_GAME_MODE);
  ofRunApp(new testApp());
  return 0;
}
