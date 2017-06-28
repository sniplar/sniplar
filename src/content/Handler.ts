'use strict';

const path = require('path');
const jQuery = require('jquery');

export class Handler {
    pageContent(site = 'main') {
        jQuery.get('file://' + path.join(__dirname, '..', 'view', site, '.html'), function(data) {
            jQuery('#main-content').html(data);
        });
    }
}
