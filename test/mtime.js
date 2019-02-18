const fs = require('fs');
const assert = require('assert').strict;
const util = require('util');

const nanostat = require('nanostat');

BigInt.prototype.toJSON = function() {
  return this.toString();
}

const stat = nanostat.statSync('package.json');
console.log('stat()ted package.json');

/*console.log(`mtime: ${stat.mtimeHigh}.${stat.mtimeLow}`);
console.log(`atime: ${stat.atimeHigh}.${stat.atimeLow}`);
console.log(`ctime: ${stat.ctimeHigh}.${stat.ctimeLow}`);
console.log(`mtime: ${stat.mtime}`);*/

const seconds = Number(stat.mtimeMs / 1000n);
const nanoseconds = Number(stat.mtimeNs);
console.log(`epoch time (seconds.nanoseconds): ${seconds}.${nanoseconds}`);

const date = new Date(Number(stat.mtimeMs));
//date.setUTCSeconds(seconds);
console.log(`date: ${date}`);
