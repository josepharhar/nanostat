//module.exports = require('./build/Release/nanostat');
const internal = require('./build/Release/nanostat');

module.exports.statSync = internal.statSync;
module.exports.lstatSync = internal.lstatSync;

module.exports.stat = function(filepath, callback) {
  try {
    callback(undefined, internal.statSync(filepath));
  } catch (err) {
    callback(err);
  }
};
module.exports.lstat = function(filepath, callback) {
  try {
    callback(undefined, internal.lstatSync(filepath));
  } catch (err) {
    callback(err);
  }
};
