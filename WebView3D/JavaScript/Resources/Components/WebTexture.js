"atomic component";

const BROWSER_WIDTH = 1024;
const BROWSER_HEIGHT = 1024;
const BROWSER_URL = "https://www.youtube.com/watch?v=dQw4w9WgXcQ";

// First create a web texture and set filtering mode
var webTexture = new WebView.WebTexture2D();
var texture2D = webTexture.texture2D;
texture2D.filterMode = Atomic.TextureFilterMode.FILTER_TRILINEAR;

// Setup a simple material for the web texture
var webMaterial = new Atomic.Material();
webMaterial.setTechnique(0, Atomic.cache.getResource("Technique", "Techniques/DiffEmissive.xml"));
webMaterial.setTexture(Atomic.TextureUnit.TU_EMISSIVE, texture2D);
webMaterial.setShaderParameter("MatEmissiveColor", "1 1 1 1");

// Create web client with pluggable handlers
var webClient = new WebView.WebClient();
// Set our render handler to be the WebTexture2D we created above
webClient.webRenderHandler = webTexture;
// Create the browser!
webClient.createBrowser(BROWSER_URL, BROWSER_WIDTH, BROWSER_HEIGHT);

exports.component = function(self) {

  var camera = self.node.scene.getComponent("Camera", true);
  var octree = self.node.scene.getComponent("Octree", true);

  // assign the web material to our model component
  var model = self.node.getComponent("StaticModel");
  model.setMaterial(webMaterial);

  var hm = true;

  // update function
  self.update = function(timeStep) {

    // 3D web texture interaction
    var mousePos = Atomic.input.getMousePosition();

    if (Atomic.input.getKeyPress(Atomic.KEY_L)) {

        hm = !hm;
    }

    if (!hm)
        return;


    // normalize x/y
    mousePos[0] /= Atomic.graphics.width;
    mousePos[1] /= Atomic.graphics.height;

    // calculate the screen ray at the mouse point
    var ray = camera.getScreenRay(mousePos[0], mousePos[1]);

    var result = octree.rayCastSingle(ray, Atomic.RayQueryLevel.RAY_TRIANGLE_UV, Atomic.M_INFINITY, Atomic.DRAWABLE_GEOMETRY);

    if (result) {

      var uv = result.textureUV;
      var x = uv[0] * BROWSER_WIDTH;
      var y = uv[1] * BROWSER_WIDTH;

      webClient.sendMouseMoveEvent(x, y, 0);

      if (Atomic.input.getMouseButtonPress(Atomic.MOUSEB_LEFT)) {
        webClient.sendMousePressEvent(x, y);
      }

    }

    if (Atomic.input.mouseMoveWheel) {

      webClient.sendMouseWheelEvent(0, 0, 0, 0, -Atomic.input.mouseMoveWheel);

    }
  };
};
