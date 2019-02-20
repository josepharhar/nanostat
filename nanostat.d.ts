declare namespace nanostat {
  interface Stat {
    stat(filepath: string, callback: (stats: Object) => void): void;
    lstat(filepath: string, callback: (stats: Object) => void): void;
    statSync(filepath: string): Object;
    lstatSync(filepath: string): Object;
  }
}

declare var nanostat: nanostat.Stat;
export = nanostat;
