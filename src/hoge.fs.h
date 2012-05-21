
#define stringify(x) # x

static string hogefs = stringify
(
uniform sampler2D tex;

void main() {
  gl_FragColor = gl_Color * texture2D(tex, gl_TexCoord[0].xy);
  for (int i = 0; i < 5; i++) {
  }
}
);
