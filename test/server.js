const nanostat = require('nanostat');

console.log('nanostat: ' + JSON.stringify(nanostat, null, 2));
console.log('nanostat.statSync: ' + nanostat.statSync);
console.log('nanostat.statSync(): ' + nanostat.statSync('server.js'));
