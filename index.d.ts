declare function stat(filepath: string, callback: (stats: Object) => void): void;
declare function lstat(filepath: string, callback: (stats: Object) => void): void;
declare function statSync(filepath: string): Object;
declare function lstatSync(filepath: string): Object;
