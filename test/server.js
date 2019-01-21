const fs = require('fs');
const assert = require('assert').strict;

const nanostat = require('nanostat');

BigInt.prototype.toJSON = function() {
  return this.toString();
}

const nstat = nanostat.statSync('package.json');
console.log('nanostat.statSync(package.json): '
  + JSON.stringify(nstat, null, 2));

const stat = fs.statSync('package.json', {bigint: true});
console.log('stat.statSync(package.json): '
  + JSON.stringify(stat, null, 2));

// TODO get all these to pass on all platforms

function assertEq(field) {
  assert.equal(nstat[field], stat[field],
    new Error(`${field} not equal.\n nstat: ${nstat[field]}\nfsstat: ${stat[field]}`));
}

assertEq('atimeMs');
assertEq('mtimeMs');
assertEq('ctimeMs');
assertEq('birthtimeMs');

/*assertEq('atimeNs');
assertEq('mtimeNs');
assertEq('ctimeNs');
assertEq('birthtimeNs');*/

assertEq('dev');
assertEq('mode');
assertEq('nlink');
assertEq('uid');
assertEq('gid');
assertEq('rdev');
assertEq('blksize');
assertEq('ino');
assertEq('size');
assertEq('blocks');
