'use strict';
var gulp = require('gulp');
var exec = require('child_process').execSync;

gulp.task('build',function() {
    console.log(exec('make').toString('utf8'));
});

gulp.task('clean',function() {
    console.log(exec('make clean').toString('utf8'));
});

gulp.task('test',['build'],function() {
    console.log(exec('./testparser test/test.sql').toString('utf8'));
});

gulp.task('default',['build','test'],function() {
    var watcher = gulp.watch('*.cpp', ['build']);
    watcher.on('change', function(event) {
        console.log('File ' + event.path + ' was ' + event.type + ', running tasks...');
    });
});

