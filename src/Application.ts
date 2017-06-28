'use strict';

class Application {
    static init () {
        Handler.pageContent();
    }
}

const path = require('path');
const jQuery = require('jquery');

class Handler {
    static pageContent(site = 'main') {
        jQuery.get('file://' + path.join(__dirname, '..', 'view', site + '.html'), function(data) {
            jQuery('#main-content').html(data);
        });
    }
}
