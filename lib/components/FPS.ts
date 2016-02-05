import stream = require('stream');

export class FPS extends stream.Duplex {
  fps;
  constructor(every?: number) {
    super();
    var self = this;
    // Needed to forward the portDefinitionChanged from the VideoDecode to the VideoRender
    this.on('pipe', function(source) {
      source.on('portDefinitionChanged', function(portDefinition) {
        self.emit('portDefinitionChanged', portDefinition);
      });
    });

    if (every === undefined) {
      every = 30;
    }

    this.fps = require('fps')({ every: every });
    this.fps.on('data', function(framerate) {
      console.log("Fps: ", framerate);
    });
  }
  _read() {
  };
  _write(chunk, enc, next) {
    this.fps.tick();
    this.push(chunk);
    next();
  };
}