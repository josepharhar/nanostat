# nanostat

nanostat is an npm package which provides stat() functionality with higher precision than fs.stat(), including the full nanosecond precision of the underlying stat() call in C.

Hopefully this functionality will be upstreamed to nodejs's builtin fs package, in which case this package will become a polyfill for it.

# Usage

nanostat is intended to mirror fs.stat() and similar functions as closely as possible.

```javascript
const nanostat = require('nanostat');
nanostat.stat('file.txt', (err, stats) => {
  if (err) {
    console.log(`err: ${err}`);
    return;
  }
  console.log(`epoch time (seconds.nanoseconds): ${stats.mtimeMs / 1000}.${stats.mtimeNs}`);
});
```
