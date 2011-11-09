// $Id: gmaps.js,v 1.2.2.8 2010/03/18 10:52:16 xmarket Exp $

GMAPS_TABS_NAV_TOP = 'top';
GMAPS_TABS_NAV_BOTTOM = 'bottom';
GMAPS_TABS_NAV_LEFT = 'left';
GMAPS_TABS_NAV_RIGHT = 'right';

GMAPS_GRID_ALIGN_HOR = 'hor';
GMAPS_GRID_ALIGN_VER = 'ver';

Drupal.behaviors.gmapsBehaviors = function(context) {
  //Element help
  $('a.gmaps-element-help:not(.gmaps-element-help-processed)', context).each(function (index, a) {
    setTimeout(function() {
      $(a).click(function () {
        var w=window.open(a.href, 'advanced_help_window', 'width='+Drupal.settings.gmaps.elementHelp.width+'px,height='+Drupal.settings.gmaps.elementHelp.height+'px,scrollbars,resizable');
        w.focus();
        return false;
      }).removeAttr('target').addClass('gmaps-element-help-processed');
    }, 5);
  });

  //Accordion element
  $('.form-gmaps-accordion:not(.gmaps-accordion-embedded):not(.gmaps-accordion-processed)', context).each(function (index, element) {
    var handler, init = function() {Drupal.gmaps.accordion.getAccordion(element.id, context, true);};
    if (handler = Drupal.gmaps.getLazyInitHandler('#'+ element.id, context)) {
      handler.attach('#'+ element.id, init, context);
    }
    else {
      setTimeout(init, 5);
    }
  });

  //Tabs element
  $('.form-gmaps-tabs:not(.gmaps-tabs-embedded):not(.gmaps-tabs-processed)', context).each(function (index, element) {
    var handler, init = function() {Drupal.gmaps.tabs.getTabs(element.id, context, true);};
    if (handler = Drupal.gmaps.getLazyInitHandler('#'+ element.id, context)) {
      handler.attach('#'+ element.id, init, context);
    }
    else {
      setTimeout(init, 5);
    }
  });
};

Drupal.attributes = function(attributes) {
  var t = '';
  if (attributes) {
    $.each(attributes, function(key, value) {
      t += ' '+ key +'="'+ Drupal.checkPlain(value) +'"';
    });
  }
  return t;
};

Drupal._formSetClass = function(element, cls) {
  cls = cls || [];
  if (element['#required']) {
    cls.push('required');
  }
  if (element['#attributes'] && element['#attributes']['class']) {
    cls.push(element['#attributes']['class']);
  }
  if (!element['#attributes']) {
    element['#attributes'] = {};
  }
  element['#attributes']['class'] = cls.join(' ');
};

Drupal._themeTableCell = function(cell, header) {
  var data, output = '', attributes = '';

  if (typeof(cell) == 'object') {
    data = typeof(cell['data']) != 'undefined' ? cell['data'] : '';
    header |= typeof(cell['header']) != 'undefined';
    var attr = $.extend(true, {}, cell);
    delete attr['data'];
    delete attr['header'];
    attributes = Drupal.attributes(attr);
  }
  else {
    data = cell;
  }

  if (header) {
    output = '<th'+ attributes +'>'+ data +'</th>';
  }
  else {
    output = '<td'+ attributes +'>'+ data +'</td>';
  }

  return output;
};

Drupal.hooks = Drupal.hooks || {};

Drupal.hooks.elements = Drupal.hooks.elements || {};

Drupal.hooks.elements.fieldset = {'#collapsible': false, '#collapsed': false, '#value': null};
//'#process' => array('form_expand_ahah')
Drupal.hooks.elements.checkbox = {'#return_value': 1};

Drupal._elementInfo = function($type, $refresh) {
  //static $cache;

  var $basic_defaults = {
    '#description': '',
    '#attributes': {},
    '#required': false,
    '#tree': false,
    '#parents': []
  };
  var $cache = $(document).data('_elementInfo.drupal');
  if (!$cache || $refresh) {
    $cache = {};
    $.each(Drupal.hooks.elements, function($element_type, $info) {
      $cache[$element_type] = $.extend(true, {}, $basic_defaults, $info);
    });
    $(document).data('_elementInfo.drupal', $cache);
  }

  return $.extend(true, {}, $cache[$type]);
};

Drupal.elementChild = function($key) {
  return typeof($key) == 'undefined' || $key === null || !$key.length || $key.charAt(0) != '#';
};

Drupal.elementChildren = function($element) {
  var $children = [];
  $.each($element, function($key, $value) {
    if (Drupal.elementChild($key)) {
      $children.push($key);
    }
  });
  return $children;
};

Drupal.render = function($elements) {
  var self = this;
  if ($elements === null || (typeof($elements['#access']) != 'undefined' && !$elements['#access'])) {
    return null;
  }
  
  $elements = $.extend(true, {}, $elements);

  // If the default values for this element haven't been loaded yet, populate
  // them.
  if (typeof($elements['#defaults_loaded']) == 'undefined' || !$elements['#defaults_loaded']) {
    var $info;
    if ((typeof($elements['#type']) != 'undefined' && $elements['#type'] !== null && $elements['#type'].length) && ($info = Drupal._elementInfo($elements['#type']))) {
      $elements = $.extend(true, $info, $elements);
    }
    $elements['#defaults_loaded'] = true;
  }

  // Make any final changes to the element before it is rendered. This means
  // that the $element or the children can be altered or corrected before the
  // element is rendered into the final text.
  if (typeof($elements['#pre_render']) != 'undefined') {
    $.each($elements['#pre_render'], function() {
      $elements = this($elements);
    });
  }
  
  var $content = '';
  $elements = $.extend({'#title': '', '#description': ''}, $elements);
  if (typeof($elements['#children']) == 'undefined') {
    var $children = Drupal.elementChildren($elements);
    // Render all the children that use a theme function.
    if (typeof($elements['#theme']) != 'undefined' && !$elements['#theme_used']) {
      $elements['#theme_used'] = true;

      var $previous = {};
      $.each(['#value', '#type', '#prefix', '#suffix'], function(i, $key) {
        $previous[$key] = typeof($elements[$key]) != 'undefined' ? $elements[$key] : null;
      });
      // If we rendered a single element, then we will skip the renderer.
      if (!$children.length) {
        $elements['#printed'] = true;
      }
      else {
        $elements['#value'] = '';
      }
      $elements['#type'] = 'markup';

      delete $elements['#prefix'];
      delete $elements['#suffix'];
      $content = Drupal.theme($elements['#theme'], $elements);

      $.each(['#value', '#type', '#prefix', '#suffix'], function(i, $key) {
        $elements[$key] = typeof($previous[$key]) != 'undefined' ? $previous[$key] : null;
      });
    }
    // Render each of the children using drupal_render and concatenate them.
    if ($content === null || !$content.length) {
      $.each($children, function(i, $key) {
        $content += Drupal.render($elements[$key]);
      });
    }
  }
  if ($content !== null && $content.length) {
    $elements['#children'] = $content;
  }

  // Until now, we rendered the children, here we render the element itself
  if (typeof($elements['#printed']) == 'undefined') {
    $content = Drupal.theme((typeof($elements['#type']) != 'undefined' && $elements['#type'] !== null && $elements['#type'].length) ? $elements['#type'] : 'markup', $elements);
    $elements['#printed'] = true;
  }

  if ($content !== null && $content.length) {
    // Filter the outputted content and make any last changes before the
    // content is sent to the browser. The changes are made on $content
    // which allows the output'ed text to be filtered.
    if (typeof($elements['#post_render']) != 'undefined') {
      $.each($elements['#post_render'], function(i, fn) {
        $content = fn($content, $elements);
      });
    }
    var $prefix = typeof($elements['#prefix']) != 'undefined' ? $elements['#prefix'] : '';
    var $suffix = typeof($elements['#suffix']) != 'undefined' ? $elements['#suffix'] : '';
    return $prefix + $content + $suffix;
  }
};

Drupal.theme.prototype.box = function(title, content, region) {
  var output = '<h2 class="title">'+ title +'</h2><div>'+ content +'</div>';
  return output;
};

Drupal.theme.prototype.itemList = function(items, title, type, attributes) {
  var output = '<div class="item-list">';
  if (title && title.length) {
    output += '<h3>'+ title +'</h3>';
  }
  
  type = type || 'ul';

  if (items && items.length) {
    output += '<'+ type + Drupal.attributes(attributes) +'>';
    $.each(items, function(i, item) {
      attributes = {};
      var children = [];
      var data = '';
      if (typeof(item) == 'object') {
        $.each(item, function(key, value) {
          if (key == 'data') {
            data = value;
          }
          else if (key == 'children') {
            children = value;
          }
          else {
            attributes[key] = value;
          }
        });
      }
      else {
        data = item;
      }
      if (children.length) {
        data += Drupal.theme.prototype.itemList(children, null, type, attributes); // Render nested list
      }
      if (i == 0) {
        attributes['class'] = (!attributes['class'] || !attributes['class'].length) ? 'first' : (attributes['class'] +' first');
      }
      if (i == (items.length - 1)) {
        attributes['class'] = (!attributes['class'] || !attributes['class'].length) ? 'last' : (attributes['class'] +' last');
      }
      output += '<li'+ Drupal.attributes(attributes) +'>'+ data +"</li>\n";
    });
    output += '</'+ type + '>';
  }
  output += '</div>';
  return output;
};

Drupal.theme.prototype.gmapsGrid = function(items, options, title) {
  if (!items || !items.length) {
    return '';
  }
  
  var cols = options['columns'];
  var attributes = (typeof(options['attributes']) != 'undefined' && options['attributes'] !== null) ? options['attributes'] : {};
  var header = (typeof(options['header']) != 'undefined' && options['header'] !== null) ? options['header'] : false;
  
  attributes['class'] = (typeof(options['class']) == 'undefined' || options['class'] === null) ? 'gmaps-grid' : ('gmaps-grid '+ attributes['class']);
  
  var output = '<table'+ Drupal.attributes(attributes) +">\n";
  
  if (title && title.length) {
    output += '<caption>'+ title +"</caption>\n";
  }
  output += "<tbody>\n";
  
  var rows = new Array();
  
  //set up grid
  if (options['alignment'] == GMAPS_GRID_ALIGN_HOR) {
    var row = new Array();
    var row_count = 0;
    $.each(items, function(count, cell) {
      row.push(cell);
      row_count++;
      if ((count + 1) % cols == 0) {
        rows.push(row);
        row = new Array();
        row_count = 0;
      }
    });
    if (row.length) {
      // Fill up the last line.
      for (var i = 0; i < (cols - row_count); i++) {
        row.push('');
      }
      rows.push(row);
    }
  }
  else {
    var num_rows = Math.floor(items.length) / cols;
    // The remainders are the 'odd' columns that are slightly longer.
    var remainders = items.length % cols;
    var row = 0;
    var col = 0;
    $.each(items, function(count, cell) {
      if (typeof(rows[row]) == 'undefined') {
        rows[row] = new Array();
      }
      rows[row][col] = cell;
      row++;

      if (!remainders && row == num_rows) {
        row = 0;
        col++;
      }
      else if (remainders && row == num_rows + 1) {
        row = 0;
        col++;
        remainders--;
      }
    });
    for (var i = 0; i <= rows[0].length; i++) {
      // This should be string so that's okay :)
      if (typeof(rows[rows.length - 1][i]) == 'undefined') {
        rows[rows.length - 1][i] = '';
      }
    }
  }
  
  var flip = {'even': 'odd', 'odd': 'even'};
  var cls = 'even';
  $.each(rows, function(row_number, row) {
    var row_class = 'row-'+ (row_number + 1) +' row-'+ cls;
    cls = flip[cls];
    if (row_number == 0) {
      row_class += ' row-first';
    }
    if (rows.length == (row_number + 1)) {
      row_class += ' row-last';
    }
    output += '<tr class="'+ row_class +'">';
    
    var flip_col = {'even': 'odd', 'odd': 'even'};
    var class_col = 'even';
    $.each(row, function(col_number, cell) {
      var cell_class = 'col-'+ (col_number + 1) +' col-'+ class_col;
      class_col = flip_col[class_col];
      if (col_number == 0) {
        cell_class += ' col-first';
      }
      if (row.length == (col_number + 1)) {
        cell_class += ' col-last';
      }
      
      if (typeof(cell) == 'object') {
        if (typeof(cell['class']) != 'undefined') {
          cell['class'] += ' '+ cell_class;
        }
        else {
          cell['class'] = cell_class;
        }
      }
      else {
        cell = {'data': cell, 'class': cell_class};
      }
      
      output += Drupal._themeTableCell(cell, header);
    });
    
    output += "</tr>\n";
  });
  
  output += "</tbody></table>\n";
  return output;
}

Drupal.theme.prototype.fieldset = function(element) {
  if (element['#collapsible']) {
    if (typeof(element['#attributes']) == 'undefined') {
      element['#attributes'] = {};
    }
    if (typeof(element['#attributes']['class']) == 'undefined') {
      element['#attributes']['class'] = '';
    }

    element['#attributes']['class'] += ' collapsible';
    if (element['#collapsed']) {
      element['#attributes']['class'] += ' collapsed';
    }
  }

  return '<fieldset'+ Drupal.attributes(element['#attributes']) +'>'+ (element['#title'] ? '<legend>'+ element['#title'] +'</legend>' : '') +
    (typeof(element['#description']) != 'undefined' && element['#description'].length ? '<div class="description">'+ element['#description'] +'</div>' : '') +
    (element['#children'] ? element['#children'] : '') + (typeof(element['#value']) != 'undefined' ? element['#value'] : '') +"</fieldset>\n";
};

Drupal.theme.prototype.form_element = function(element, value) {
  var output = '<div class="form-item"';
  var hasId = typeof(element['#id']) != 'undefined' && element['#id'] !== null && element['#id'].length;
  if (hasId) {
    output += ' id="'+ element['#id'] +'-wrapper"';
  }
  output += ">\n";
  var required = (typeof(element['#required']) != 'undefined' && element['#required'] !== null) ? '<span class="form-required" title="'+ Drupal.t('This field is required.') +'">*</span>' : '';

  if (typeof(element['#title']) != 'undefined' && element['#title'] !== null && element['#title'].length) {
    var title = element['#title'];
    if (hasId) {
      output += ' <label for="'+ element['#id'] +'">'+ Drupal.t('!title: !required', {'!title': title, '!required': required}) +"</label>\n";
    }
    else {
      output += ' <label>'+ Drupal.t('!title: !required', {'!title': title, '!required': required}) +"</label>\n";
    }
  }

  output += ' '+ value +"\n";

  if (typeof(element['#description']) != 'undefined' && element['#description'] !== null && element['#description'].length) {
    output += ' <div class="description">'+ element['#description'] +"</div>\n";
  }

  output += "</div>\n";

  return output;
};

Drupal.theme.prototype.checkbox = function(element) {
  Drupal._formSetClass(element, ['form-checkbox']);
  var checkbox = '<input ';
  checkbox += 'type="checkbox" ';
  checkbox += element['#name'] ? 'name="'+ element['#name'] +'" ' : '';
  checkbox += element['#id'] ? 'id="'+ element['#id'] +'" ' : '';
  checkbox += 'value="'+ element['#return_value'] +'" ';
  checkbox += element['#value'] ? ' checked="checked" ' : ' ';
  checkbox += Drupal.attributes(element['#attributes']) +' />';

  if (element['#title'] != null) {
    checkbox = '<label class="option"'+ (element['#id'] ? ' for="'+ element['#id'] +'"' : '') +'>'+ checkbox +' '+ element['#title'] +'</label>';
  }

  delete element['#title'];
  return Drupal.theme('form_element', element, checkbox);
};

Drupal.theme.prototype.gmaps_accordion = function(element) {
  var hasChildren = typeof(element['#children']) != 'undefined' && element['#children'] !== null && element['#children'].length;
  var output = '<div id="'+ element['#id'] +'" '+ Drupal.attributes(element['#attributes']) +'>' +
    (hasChildren ? ('<div class="gmaps-accordion">'+ element['#children'] +'</div>') : '') +'</div>'+
    (typeof(element['#value']) != 'undefined' && element['#value'] !== null ? element['#value'] : '');

  return Drupal.theme('form_element', element, output);
};

Drupal.theme.prototype.gmaps_accordion_panel = function(element) {
  if (typeof(element['#attributes']) == 'undefined' || element['#attributes'] === null) {
    element['#attributes'] = {};
  }
  if (typeof(element['#attributes']['class']) == 'undefined' || element['#attributes']['class'] === null) {
    element['#attributes']['class'] = '';
  }
  element['#attributes']['class'] = 'gmaps-accordion-header'+ (element['#accordion_id'] ? ' '+ element['#accordion_id'] : '') +
    ((typeof(element['#active_panel']) != 'undefined' && element['#active_panel']) ? ' ui-state-active' : '') +' '+ element['#attributes']['class'];
  var output = '<h3 '+ Drupal.attributes(element['#attributes']) +'><a href="#'+ element['#id'] +'">'+ element['#title'] +"</a></h3>\n";
  output += '<div id="'+ element['#id'] +'" class="gmaps-accordion-panel'+ ((typeof(element['#active_panel']) != 'undefined' && element['#active_panel']) ? ' ui-accordion-content-active' : '') +"\">\n";
  
  if (typeof(element['#description']) != 'undefined' && element['#description'] !== null && element['#description'].length) {
    output += '<div class="description">'+ element['#description'] +"</div>\n";
  }
  output += '<div id="'+ element['#id'] +'-content">';
  output += ((typeof(element['#children']) != 'undefined' && element['#children'] !== null && element['#children'].length) ? element['#children'] : '');
  output += ((typeof(element['#value']) != 'undefined' && element['#value'] !== null) ? element['#value'] : '');
  output += "</div></div>\n";
  
  return output;
};

Drupal.theme.prototype.gmaps_tabs = function(element) {
  var output = '<div id="'+ element['#id'] +'" '+ Drupal.attributes(element['#attributes']) +'><div class="gmaps-tabs">';
  
  //tabs
  var nav = "<ul class=\"gmaps-tabs-nav\">\n";
  var lineend = (element['#nav_position'] == GMAPS_TABS_NAV_TOP || element['#nav_position'] == GMAPS_TABS_NAV_BOTTOM) ? "\n" : '';
  $.each(Drupal.elementChildren(element), function() {
    if (typeof(element[this]['#type']) != 'undefined' && element[this]['#type'] !== null && element[this]['#type'] == 'gmaps_tabs_panel') {
      var panel = element[this];
      nav += '<li class="gmaps-tabs-tab '+ element['#id'] +'-tab'+ (panel['#selected_panel'] ? ' ui-tabs-selected' : '') +'"><a href="#'+ panel['#id'] +'"><span>'+ panel['#title'] +'</span></a></li>'+ lineend;
    }
    
  });
  nav += "</ul>\n";
  
  if (element['#nav_position'] != GMAPS_TABS_NAV_BOTTOM) {
    output += nav;
  }
  output += element['#children'];
  if (element['#nav_position'] == GMAPS_TABS_NAV_BOTTOM) {
    output += nav;
  }
  output += '</div><div class="gmaps-tabs-clear-nav"></div>'+ ((typeof(element['#value']) != 'undefined' && element['#value'] !== null) ? element['#value'] : '') +"</div>\n";
  
  return Drupal.theme('form_element', element, output);
};

Drupal.theme.prototype.gmaps_tabs_panel = function(element) {
  if (typeof(element['#attributes']) == 'undefined' || element['#attributes'] === null) {
    element['#attributes'] = {};
  }
  if (typeof(element['#attributes']['class']) == 'undefined' || element['#attributes']['class'] === null) {
    element['#attributes']['class'] = '';
  }
  element['#attributes']['class'] = 'gmaps-tabs-panel '+ element['#tabs_id'] +'-panel' +
    ((typeof(element['#selected_panel']) == 'undefined' || !element['#selected_panel']) ? ' ui-tabs-hide' : '') +' '+ element['#attributes']['class'];
  var output = '<div id="'+ element['#id'] +'" '+ Drupal.attributes(element['#attributes']) +">\n";

  if (typeof(element['#description']) != 'undefined' && element['#description'] !== null && element['#description'].length) {
    output += ' <div class="description">'+ element['#description'] +"</div>\n";
  }
  output += '<div id="'+ element['#id'] +'-content">';
  output += ((typeof(element['#children']) != 'undefined' && element['#children'] !== null && element['#children'].length) ? element['#children'] : '');
  output += ((typeof(element['#value']) != 'undefined' && element['#value'] !== null) ? element['#value'] : '');
  output += "</div></div>\n";
  
  return output;
};

Drupal.theme.prototype.markup = function(element) {
  return ((typeof(element['#value']) != 'undefined' && element['#value'] !== null) ? element['#value'] : '') +
    ((typeof(element['#children']) != 'undefined' && element['#children'] !== null) ? element['#children'] : '');
}

GMapsLazyInitHandlerFieldset = function() {
  this.check = function(selector, context) {
    var parent = $(selector, context).parents('fieldset.collapsed:first');
    return (parent.length && !$('input.error, textarea.error, select.error', parent).length) ? parent : false;
  };
  
  this.checkContext = function(context, selector) {
    var mostInner = $('fieldset.collapsed:has('+ selector +'):last', context);
    return (mostInner.length && !$('input.error, textarea.error, select.error', mostInner).length) ? mostInner : false;
  };
  
  this.attach = function(selector, callback, context) {
    context = context || document;
    $(selector, context).parents('fieldset.collapsed:first').one('mouseover', function(){
      $('legend a', this).one('click', function() {setTimeout(callback, 300);});
    });
    
  };
};

GMapsLazyInitHandlerAccordion = function() {
  this.check = function(selector, context) {
    var parent = $(selector, context).parents('.gmaps-accordion-panel:not(.ui-accordion-content-active):first');
    return parent.length ? parent : false;
  };
  
  this.checkContext = function(context, selector) {
    var mostInner = $('.gmaps-accordion-panel:not(.ui-accordion-content-active):has('+ selector +'):last', context);
    return mostInner.length ? mostInner : false;
  };
  
  this.attach = function(selector, callback, context) {
    context = context || document;
    var accordion = $('.gmaps-accordion:first', $(selector, context).parents('.gmaps-accordion-panel:not(.ui-accordion-content-active)').parents('.form-gmaps-accordion:first'));
    var fn = function(e, ui) {
      if ($(selector, ui.newContent).length) {
        callback();
        accordion.unbind('accordionchange', fn);
      }
    };
    accordion.bind('accordionchange', fn);
  };
};

GMapsLazyInitHandlerTabs = function() {
  this.check = function(selector, context) {
    var parent = $(selector, context).parents('.gmaps-tabs-panel.ui-tabs-hide:first');
    return parent.length ? parent : false;
  };
  
  this.checkContext = function(context, selector) {
    var mostInner = $('.gmaps-tabs-panel.ui-tabs-hide:has('+ selector +'):last', context);
    return mostInner.length ? mostInner : false;
  };
  
  this.attach = function(selector, callback, context) {
    context = context || document;
    var tabs = $('.gmaps-tabs:first', $(selector, context).parents('.form-gmaps-tabs:first'));
    var fn = function(e, ui) {
      if ($(selector, ui.panel).length) {
        callback();
        tabs.unbind('tabsshow', fn);
      }
    };
    tabs.bind('tabsshow', fn);
  };
};

Drupal.gmaps = Drupal.gmaps || new (function() {
  var self = this, jsre = /=\?(&|$)/g;

  this.globalAjaxParams = {
    'gmaps-async-api': '1'
  };
  var ajaxQueueIndex = 0;
  this.ajaxQueue = {};
  
  this.ajaxTests = {
    'css': {
      'drupal': {
        're': new RegExp('^'+ location.host + Drupal.settings.basePath +'modules'),
        'test': function() {return true;}
      },
      'gmaps': {
        're': /gmaps.*?\/misc\/.+/,
        'test': function() {return true;}
      },
      'gmaps_admin': {
        're': /gmaps\/misc\/gmaps-admin\.css.*$/,
        'test': function() {
          if ($('#gmaps-icon-color-form').length) {
            return true;
          }
        }
      }
    },
    'js': {
      'drupal': {
        're': /\/drupal\.js/,
        'test': function() {return true;}
      },
      'jquery': {
        're': /\/jquery\.js/,
        'test': function(file) {return true;}
      },
      'jqueryForm': {
        're': /\/jquery\.form\.js/,
        'test': function() {return typeof(jQuery.ajaxSubmit) != 'undefined';}
      },
      'jqueryUITabs': {
        're': /\/jquery\.ui\/ui\/ui\.tabs\.js/,
        'test': function() {return $.ui && $.ui.tabs;}
      },
      'jqueryUIAccordion': {
        're': /\/jquery\.ui\/ui\/ui\.accordion\.js/,
        'test': function() {return $.ui && $.ui.accordion;}
      },
      'ahah': {
        're': /\/ahah\.js/,
        'test': function() {return typeof(Drupal.behaviors.ahah) != 'undefined';}
      },
      'fieldset': {
        're': /\/collapse\.js/,
        'test': function() {return typeof(Drupal.behaviors.collapse) != 'undefined';}
      },
      'form': {
        're': /\/form\.js/,
        'test': function() {return typeof(Drupal.behaviors.multiselectSelector) != 'undefined';}
      },
      'autocomplete': {
        're': /\/autocomplete\.js/,
        'test': function() {return typeof(Drupal.behaviors.autocomplete) != 'undefined';}
      },
      'progress': {
        're': /\/progress\.js/,
        'test': function() {return typeof(Drupal.progressBar) != 'undefined';}
      },
      'tabledrag': {
        're': /\/tabledrag\.js/,
        'test': function() {return typeof(Drupal.behaviors.tableDrag) != 'undefined';}
      },
      'tableheader': {
        're': /\/tableheader\.js/,
        'test': function() {return typeof(Drupal.behaviors.tableHeader) != 'undefined';}
      },
      'tableselect': {
        're': /\/tableselect\.js/,
        'test': function() {return typeof(Drupal.behaviors.tableSelect) != 'undefined';}
      },
      'teaser': {
        're': /\/teaser\.js/,
        'test': function() {return typeof(Drupal.behaviors.teaser) != 'undefined';}
      },
      'textarea': {
        're': /\/textarea\.js/,
        'test': function() {return typeof(Drupal.behaviors.textarea) != 'undefined';}
      },
      'farbtastic': {
        're': /\/farbtastic\.js/,
        'test': function() {return typeof(jQuery.farbtastic) != 'undefined';}
      },
      'gmaps': {
        're': /gmaps\/misc\/gmaps\.js/,
        'test': function() {return typeof(Drupal.gmaps) != 'undefined';}
      },
      'gmaps_icon_select': {
        're': /gmaps\/misc\/gmaps-icon-select\.js/,
        'test': function() {return typeof(GMapsIconSelectElement) != 'undefined';}
      },
      'gmaps_geocoder': {
        're': /gmaps\/misc\/gmaps-geocoder-element\.js/,
        'test': function() {return typeof(GMapsGeocoderElement) != 'undefined';}
      },
      'gmaps_address': {
        're': /gmaps\/misc\/gmaps-address-element\.js/,
        'test': function() {return typeof(GMapsAddressElement) != 'undefined';}
      },
      'gmaps_point': {
        're': /gmaps\/misc\/gmaps-point-element\.js/,
        'test': function() {return typeof(GMapsPointElement) != 'undefined';}
      },
      'gmaps_anp': {
        're': /'gmaps\/misc\/gmaps-anp-element\.js'/,
        'test': function() {return typeof(GMapsAnpElement) != 'undefined';}
      },
      'gmaps_map': {
        're': /gmaps\/misc\/gmaps-map\.js/,
        'test': function() {return typeof(GMapsMapItem) != 'undefined';}
      },
      'gmaps_icon': {
        're': /gmaps\/misc\/gmaps-icon\.js/,
        'test': function() {return typeof(Drupal.gmaps.icon) != 'undefined';}
      },
      'gmaps_marker': {
        're': /gmaps\/misc\/gmaps-marker\.js/,
        'test': function() {return typeof(Drupal.gmaps.marker) != 'undefined';}
      },
      'dhtml_menu': {
        're': /\/dhtml_menu\/dhtml_menu\.js/,
        'test': function() {return typeof(Drupal.dhtmlMenu) != 'undefined';}
      },
      'lightbox': {
        're': /\/js\/lightbox\.js/,
        'test': function() {return typeof(Lightbox) != 'undefined';}
      }
    }
  };

  this.lazyInitHandlers = {
    'fieldset': new GMapsLazyInitHandlerFieldset(),
    'tabs': new GMapsLazyInitHandlerTabs(),
    'accordion': new GMapsLazyInitHandlerAccordion()
  };
  
  var getSubLazyInitHandler = function(context, selector, skip) {
    var handler, subcontext;
    $.each(self.lazyInitHandlers, function() {
      if ($.inArray(this, skip) == -1) {
        skip.push(this);
        if (subcontext = this.checkContext(context, selector)) {
          handler = getSubLazyInitHandler(subcontext, selector, skip) || this;
          return false;
        }
      }
    });
    return handler;
  };
  
  this.getLazyInitHandler = function(selector, context) {
    context = context || document;
    var handler, handlerContext, skip = [];
    $.each(self.lazyInitHandlers, function() {
      if ($.inArray(this, skip) == -1) {
        skip.push(this);
        if (handlerContext = this.check(selector, context)) {
          handler = getSubLazyInitHandler(handlerContext, selector, skip) || this;
          return false;
        }
      }
    });
    return handler;
  };
  
  var checkAjaxCSSFiles = function(files, css) {
    var newFiles = {};
    $.each(files, function(file, media) {
      if (typeof(css[file]) == 'undefined') {
        var checked = false;
        $.each(self.ajaxTests.css, function(i, test) {
          if (test.re.test(file)) {
            checked = true;
            //tests should return true, when the 'content' already exists
            if (!test.test()) {
              newFiles[file] = media;
            }
            return false;
          }
        });
        if (!checked) {
          newFiles[file] = media;
        }
      }
    });
    return newFiles;
  };
  
  var processAjaxCSS = function(files, css) {
    var head = document.getElementsByTagName("head")[0] || document.documentElement;
    var link, newFiles = checkAjaxCSSFiles(files, css);
    $.each(newFiles, function(file, media) {
      link = document.createElement('link');
      link.href = file;
      link.type = 'text/css';
      link.rel = 'stylesheet';
      link.media = media;
      head.appendChild(link);
      css[file] = media;
    });
  };
  
  var checkAjaxScriptFiles = function(files, scripts, scriptIds) {
    var newFiles = [];
    var tests = $.extend({}, self.ajaxTests.js);
    $.each(files, function(i, data) {
      if ($.inArray(data.url, scripts) == -1 && (!data.id.length || $.inArray(data.id, scriptIds) == -1)) {
        var checked = false;
        $.each(tests, function(i, test) {
          if (test.re.test(data.url)) {
            checked = true;
            delete tests[i];
            //tests should return true, when the 'content' already exists
            if (!test.test()) {
              newFiles.push(data);
            }
            return false;
          }
        });
        if (!checked) {
          newFiles.push(data);
        }
      }
    });
    return newFiles;
  };
  
  var processAjaxScript = function(files, scripts, scriptIds, cache, callback) {
    if (!files.length) {
      callback();
      return;
    }
    var data = files.shift(), ajaxSuccess;
    if ($.inArray(data.url, scripts) == -1 && (!data.id.length || $.inArray(data.id, scriptIds) == -1)) {
      scripts.push(data.url);
      scriptIds.push(data.id);
      if (data.url.match(jsre)) {
        var id = 'ajax'+ ajaxQueueIndex++;
        data.url = data.url.replace(jsre, "=Drupal.gmaps.ajaxQueue." + id + "$1");
        Drupal.gmaps.ajaxQueue[id] = function(){
          processAjaxScript(files, scripts, scriptIds, cache, callback);
        };
        ajaxSuccess = function(){};
      }
      else {
        ajaxSuccess = function() {processAjaxScript(files, scripts, scriptIds, cache, callback)};
      }
      $.ajax({
        type: "GET",
        'url': data.url,
        data: null,
        'cache': cache,
        success: ajaxSuccess,
        dataType: 'script'
      });
    }
    else {
      processAjaxScript(files, scripts, scriptIds, cache, callback);
    }
  };
  
  var processAjaxInlineCode = function(codes, callback) {
    if (!code.length) {
      callback();
      return;
    }
    var code = codes.shift();
    $.globalEval(code);
    //give some time for script to execute
    setTimeout(function() {processAjaxInlineCode(codes, callback);}, 75);
  };
  
  var processAjax = function(r, cache, callback) {
    var head = document.getElementsByTagName("head")[0] || document.documentElement;
    var css = {}, scripts = [], scriptIds = [], headScripts = [], headInline = [], file, i, data;
    callback = (typeof(callback) == 'undefined' || callback === null || !$.isFunction(callback)) ? function(){} : callback;
    
    //head
    if (typeof(r.head) != 'undefined') {
      var dom = $(r.head);

      $(dom).filter('script').each(function() {
        if (this.src.length) {
          headScripts.push({url: this.src, id: this.id});
        }
        else {
          headInline.push($(this).text());
        }
      });
      if (headScripts.length) {
        if (typeof(r.js) == 'undefined') {
          r.js = {};
        }
        if (typeof(r.js.files) == 'undefined') {
          r.js.files = [];
        }
        $.each(headScripts.reverse(), function(i, data) {
          r.js.files.unshift(data);
        });
      }
      if (headInline.length) {
        if (typeof(r.js) == 'undefined') {
          r.js = {};
        }
        if (typeof(r.js.inline) == 'undefined') {
          r.js.inline = [];
        }
        $.each(headInline.reverse(), function(i, code) {
          r.js.inline.unshift(code);
        });
      }

      $(dom).filter('link[type=text/css]').each(function() {
        if (typeof(r.css) == 'undefined') {
          r.css = {};
        }
        if (typeof(r.css.head) == 'undefined') {
          r.css.head = {};
        }
        r.css.head[this.href] = (this.media != null && this.media.length) ? this.media : 'all';
      });
      $(dom).filter(':not(script):not(link[type=text/css])').each(function() {
        //unlike $.globalEval(), we can use appendChild, because we don't want to remove it.
        head.appendChild(this);
      });
    }

    //css
    if (typeof(r.css) != 'undefined') {
      $('link').each(function() {
        if (this.href.length) {
          css[this.href] = (this.media != null && this.media.length) ? this.media : "all";
        }
      });
      //head
      if (typeof(r.css.head) != 'undefined') {
        processAjaxCSS(r.css.head, css);
      }
      //module
      if (typeof(r.css.module) != 'undefined') {
        processAjaxCSS(r.css.module, css);
      }
      //misc
      if (typeof(r.css.misc) != 'undefined') {
        processAjaxCSS(r.css.misc, css);
      }
      //theme
      if (typeof(r.css.theme) != 'undefined') {
        processAjaxCSS(r.css.theme, css);
      }
    }
    
    //js
    if (typeof(r.js) != 'undefined') {
      //settings
      var extendSettings = function() {
        if (typeof(r.js.setting) != 'undefined') {
          //deep extended
          Drupal.settings = $.extend(true, Drupal.settings, r.js.setting);
        }
        callback();
      };
      
      //inline code
      var inlineCallback = function() {
        if (typeof(r.js.inline) != 'undefined') {
          processAjaxInlineCode(r.js.inline, extendSettings);
        }
        else {
          extendSettings();
        }
      };
  
      //scripts
      if (typeof(r.js.files) != 'undefined') {
        $('script').each(function() {
          if (this.src.length) {
            scripts.push(this.src);
            scriptIds.push(this.id);
          }
        });
        var newFiles = checkAjaxScriptFiles(r.js.files, scripts, scriptIds);
        processAjaxScript(newFiles, scripts, scriptIds, cache, inlineCallback);
      }
      else {
        inlineCallback();
      }
    }
    else {
      callback();
    }
  };
  
  this.prepareAjaxOptions = function(ajax) {
    var callback = ajax.success || function(){};
    
    if (typeof(ajax.type) == 'undefined' || ajax.type === null) {
      ajax.type = (typeof(ajax.data) == 'undefined' || ajax.data === null) ? 'GET' : 'POST';
    }
    ajax.dataType = 'json';
    ajax.cache = (typeof(ajax.cache) == 'undefined' || ajax.cache === null) ? true : ajax.cache;
    
    ajax.success = function(r, s) {
      if (typeof(r.ajax) != 'undefined') {
        processAjax(r.ajax, ajax.cache, function() {callback(r);});
      }
      else {
        callback(r);
      }
    };
    
    ajax.data = $.extend(true, ajax.data ? ajax.data : {}, self.globalAjaxParams);

    ajax.gmapsPrepared = true;
  };
  
  this.loadAjaxContent = function(ajax) {
    if (!ajax.gmapsPrepared) {
      self.prepareAjaxOptions(ajax);
    }

    $.ajax(ajax);
  };
  
  this.flatten = function(obj, parent, ret) {
    ret = ret || {};
    $.each(obj, function(i, val) {
      i = parent ? (parent +'['+ i +']') : i;
      if (val && typeof(val) == "object" && !val.nodeType) {
        self.flatten(val, i, ret);
      }
      else if (typeof(val) == 'undefined' || val === null) {
        ret[i] = '';
      }
      else if (val === true || val === false) {
        ret[i] = val ? 1 : 0;
      }
      else {
        ret[i] = $.isFunction(val) ? val() : val;
      }
    });
    
    return ret;
  };
  
  this.expandAhahOptions = function(o, item) {
    o.wrapper = '#'+ o.wrapper;
    o.effect = o.effect || 'none';
    o.method = o.method || 'replace';
    o.ajax = (typeof(o.ajax) != 'undefined' && o.ajax !== null) ? o.ajax : true;

    if (o.effect == 'none') {
      o.showEffect = 'show';
      o.hideEffect = 'hide';
      o.showSpeed = '';
    }
    else if (o.effect == 'fade') {
      o.showEffect = 'fadeIn';
      o.hideEffect = 'fadeOut';
      o.showSpeed = 'slow';
    }
    else {
      o.showEffect = o.effect + 'Toggle';
      o.hideEffect = o.effect + 'Toggle';
      o.showSpeed = 'slow';
    }

    var form = $(item).parents('form:first');
    if (form.length) {
      o.form = {
        form: form,
        form_action: form.attr('action'),
        form_target: form.attr('target'),
        form_encattr: form.attr('encattr')
      };
    }
  };
})();

Drupal.gmaps.accordion = Drupal.gmaps.accordion || new (function() {
  var self = this;
  this.accordions = {};
  
  this.behaviors= {};
  
  this.attachBehaviors = function(a) {
    if (a) {
      $.each(self.behaviors, function() {
        this(a);
      });
    }
  };
  
  var beforeAhah = function(id, ui, o) {
    $(ui.newHeader).addClass('gmaps-accordion-loading');
    $(ui.newContent).addClass('gmaps-accordion-content-loading');

    // Insert progressbar or throbber.
    if (o.progress.type == 'bar') {
      var progressBar = new Drupal.progressBar('ahah-progress-' + id, eval(o.progress.update_callback), o.progress.method, eval(o.progress.error_callback));
      if (o.progress.message) {
        progressBar.setProgress(-1, o.progress.message);
      }
      if (o.progress.url) {
        progressBar.startMonitoring(o.progress.url, o.progress.interval || 1500);
      }
      o.progress.element = $(progressBar.element).addClass('ahah-progress ahah-progress-bar');
      o.progress.object = progressBar;
      $(ui.newHeader).append(o.progress.element);
    }
    else if (o.progress.type == 'throbber') {
      o.progress.element = $('<div class="ahah-progress ahah-progress-throbber"><div class="throbber">&nbsp;</div></div>');
      if (o.progress.message) {
        $('.throbber', o.progress.element).after('<div class="message">' + o.progress.message + '</div>')
      }
      $(ui.newHeader).append(o.progress.element);
    }
  };
  
  var ahahSuccess = function(r, element, ui, o) {
    var wrapper = $(o.wrapper, ui.newContent);

    // Restore the previous action and target to the form.
    if (o.form) {
      o.form.form.attr('action', o.form.form_action);
      o.form.form_target ? o.form.form.attr('target', o.form.form_target) : o.form.form.removeAttr('target');
      o.form.form_encattr ? o.form.form.attr('target', o.form.form_encattr) : o.form.form.removeAttr('encattr');
    }
    
    // Manually insert HTML into the jQuery object, using $() directly crashes
    // Safari with long string lengths. http://dev.jquery.com/ticket/1152
    var new_content = $("<div/>");
    if (o.ajax) {
      r = r.data;
    }
    new_content.html(o.selector ?
      new_content
        .append(r.replace(/<script(.|\s)*?\/script>/g, ""))
        .find(o.selector) :
      r);

    // Remove the progress element.
    if (o.progress.element) {
      $(o.progress.element).remove();
    }
    if (o.progress.object) {
      o.progress.object.stopMonitoring();
    }
    if (o.cache) {
      $(ui.newContent).addClass('gmaps-accordion-ahah-processed');
    }
    $(ui.newHeader).removeClass('gmaps-accordion-loading');
    $(ui.newContent).removeClass('gmaps-accordion-content-loading');

    // Add the new content to the page.
    Drupal.freezeHeight();
    if (o.method == 'replace') {
      wrapper.empty().append(new_content);
    }
    else {
      wrapper[o.method](new_content);
    }

    // Immediately hide the new content if we're using any effects.
    if (o.showEffect != 'show') {
      new_content.hide();
    }

    // Determine what effect use and what content will receive the effect, then
    // show the new content. For browser compatibility, Safari is excluded from
    // using effects on table rows.
    if (($.browser.safari && $("tr.ahah-new-content", new_content).size() > 0)) {
      new_content.show();
    }
    else if ($('.ahah-new-content', new_content).size() > 0) {
      $('.ahah-new-content', new_content).hide();
      new_content.show();
      $(".ahah-new-content", new_content)[o.showEffect](o.showSpeed);
    }
    else if (o.showEffect != 'show') {
      new_content[o.showEffect](o.showSpeed);
    }

    // Attach all javascript behaviors to the new content, if it was successfully
    // added to the page, this if statement allows #ahah[wrapper] to be optional.
    if (new_content.parents('html').length > 0) {
      Drupal.attachBehaviors(new_content);
    }

    Drupal.unfreezeHeight();
    
    $(element).trigger('gmapsaccordionload', [ui]);
  };
  
  var ahahError = function(r, element, ui, o) {
    alert(Drupal.ahahError(r, o.url));

    // Remove the progress element.
    if (o.progress.element) {
      $(o.progress.element).remove();
    }
    if (o.progress.object) {
      o.progress.object.stopMonitoring();
    }
    // Undo hide.
    $(o.wrapper).show();
    
    $(ui.newHeader).removeClass('gmaps-accordion-loading');
    $(ui.newContent).removeClass('gmaps-accordion-content-loading');
  };
  
  this.getAccordion = function(id, context, refresh) {
    if (self.accordions[id]) {
      if (refresh) {
        self.accordions[id] = null;
      }
      else {
        return self.accordions[id];
      }
    }
    context = context || document;
    var item = $('#'+ id, context), element = $('.gmaps-accordion:first', item), s = {options: {}},
      contentSelectedClass = 'ui-accordion-content-active';
    if (typeof(Drupal.settings.gmaps.accordion) != 'undefined' && typeof(Drupal.settings.gmaps.accordion[id]) != 'undefined') {
      s = $.extend(true, s, Drupal.settings.gmaps.accordion[id]);
    }
    if (typeof(s.options.active) != 'undefined' && s.options.active !== false) {
      s.options.active = parseInt(s.options.active);
    }
    //AHAH and AJAX content
    if (typeof(s.ahahPanels) != 'undefined') {
      var contentLoader = function(e, ui) {
        if (typeof(ui.newContent) != 'undefined' && ui.newContent.length && typeof(s.ahahPanels[ui.newContent[0].id]) != 'undefined') {
          var ahahOpts = $.extend(true, {}, s.ahahOptions || {}, s.ahahPanels[ui.newContent[0].id]);

          if (!ahahOpts.cache || !$(ui.newContent).hasClass('gmaps-accordion-ahah-processed')) {
            Drupal.gmaps.expandAhahOptions(ahahOpts, item);
            beforeAhah(item[0].id, ui, ahahOpts);
            
            var ajax = {
              url: ahahOpts.url,
              type: 'POST',
              dataType: ahahOpts.ajax ? 'json' : 'html',
              cache: ahahOpts.cache,
              success: function(r, s) {
                // Sanity check for browser support (object expected).
                // When using iFrame uploads, responses must be returned as a string.
                if (ahahOpts.ajax && typeof(r) == 'string') {
                  r = Drupal.parseJson(r);
                }
                return ahahSuccess(r, element, ui, ahahOpts);
              },
              complete: function(r, s) {
                if (s == 'error' || s == 'parsererror') {
                  return ahahError(r, element, ui, ahahOpts);
                }
              }
            };
            ajax.data = {'#accordion': {id: id, panel_id: ui.newContent[0].id}};
            if (ahahOpts.data) {
              ajax.data['#accordion'].data = ahahOpts.data;
            }
            if (!ahahOpts.ajax && ahahOpts.form) {
              $.each(ahahOpts.form.form.formToArray(), function() {
                ajax.data[this.name] = this.value;
              });
            }
            ajax.data = Drupal.gmaps.flatten(ajax.data);
            
            if (ahahOpts.ajax) {
              if (ahahOpts.form) {
                Drupal.gmaps.prepareAjaxOptions(ajax);
                ahahOpts.form.form.ajaxSubmit(ajax);
              }
              else {
                Drupal.gmaps.loadAjaxContent(ajax);
              }
            }
            else {
              $.ajax(ajax);
            }
          }
        }
      };
      s.options.changestart = contentLoader;
      
      //trigger content loading, if initial page is AHAH enabled
      if (typeof(s.options.active) != 'undefined' && s.options.active !== false) {
        var initialPanel = $('.gmaps-accordion-panel:eq('+ s.options.active +')', element);
        var ahahOpts = s.ahahPanels[initialPanel[0].id];
        if (typeof(ahahOpts) != 'undefined') {
          contentLoader(null, {newContent: initialPanel, newHeader: initialPanel.prev()});
        }
      }
    }
    self.accordions[id] = element.accordion(s.options).addClass('gmaps-accordion-processed');

    //simulate 1.7.1 panel theming
    $(element).bind('accordionchange', function(e, ui) {
      ui.newContent.addClass(contentSelectedClass);
      ui.oldContent.removeClass(contentSelectedClass);
    });

    var activated = false;
    $($(element).accordion('option', 'header'), element).each(function(){
      if ($('.error', $(this).next()).not('div.error').length) {
        $(this).addClass('error');
        if (!activated) {
          $(element).accordion('activate', this);
          activated = true;
        }
      }
    });
    self.attachBehaviors(element);
    
    return self.accordions[id];
  };
  
  this.cleanId = function(id) {
    var seenIds = $().data('seenIds.accordion.gmaps.drupal');
    if (!seenIds) {
      seenIds = {};
      $('.form-gmaps-accordion').each(function() {
        seenIds[this.id] = 1;
        $('.gmaps-accordion-panel', this).each(function() {
          seenIds[this.id] = 1;
        });
      });
    }

    var newid = id.replace(/[\]\[_\s]/g, '-');

    if (typeof(seenIds[newid]) != 'undefined') {
      newid = newid +'-'+ seenIds[newid]++;
    }
    else {
      seenIds[newid] = 1;
    }
    $().data('seenIds.accordion.gmaps.drupal', seenIds);

    return newid;
  };
  
  var preRender = function(element) {
    element['#id'] = self.cleanId('accordion-'+ (element['#id'] ? element['#id'] : 'gmaps'));
    
    element['#required'] = false;
    
    if (typeof(element['#accordion']) == 'undefined' || element['#accordion'] === null) {
      element['#accordion'] = {};
    }
    
    if (typeof(element['#accordion']['active']) == 'undefined') {
      element['#accordion']['active'] = null;
    }

    //drupalized shortcuts
    if (element['#collapsible']) {
      element['#accordion']['collapsible'] = true;
      if (element['#collapsed']) {
        element['#accordion']['active'] = false;
      }
      else if (element['#accordion']['active'] === false || element['#accordion']['active'] === null) {
        element['#accordion']['active'] = 0;
      }
    }
    else {
      element['#collapsed'] = false;
      element['#accordion']['collapsible'] = false;
    }
    //1.6
    element['#accordion']['alwaysOpen'] = !element['#accordion']['collapsible'];
    
    element['#accordion']['header'] = 'h3.'+ element['#id'];
    element['#accordion']['selectedClass'] = 'ui-state-active';
    
    //fieldsets are jumping around in IE6 when set to TRUE
    if (typeof(element['#accordion']['autoHeight']) == 'undefined' || element['#accordion']['autoHeight'] === null) {
      element['#accordion']['autoHeight'] = false;
    }
    if (typeof(element['#accordion']['navigation']) == 'undefined' || element['#accordion']['navigation'] === null) {
      element['#accordion']['navigation'] = false;
    }
      
    var i = 0, panel, firstPanelId;
    $.each(Drupal.elementChildren(element), function() {
      if (typeof(element[this]['#type']) != 'undefined' && element[this]['#type'] !== null && element[this]['#type'] == 'gmaps_accordion_panel') {
        panel = element[this];
        panel['#id'] = self.cleanId(element['#id'] +'-'+ (panel['#id'] ? panel['#id'] : 'panel'));
        panel['#accordion_id'] = element['#id'];
        
        //synchronize CSS and JS
        if (panel['#active_panel']) {
          if (element['#accordion']['active'] === null) {
            element['#accordion']['active'] = i;
          }
          else if (element['#accordion']['active'] === false || element['#accordion']['active'] != i) {
            panel['#active_panel'] = false;
          }
        }
        else if (element['#accordion']['active'] !== null && element['#accordion']['active'] !== false && element['#accordion']['active'] == i) {
          panel['#active_panel'] = true;
        }
        
        if (i == 0) {
          if (typeof(panel['#attributes']) == 'undefined' || panel['#attributes'] === null) {
            panel['#attributes'] = {};
          }
          if (typeof(panel['#attributes']['class']) == 'undefined' || panel['#attributes']['class'] === null) {
            panel['#attributes']['class'] = '';
          }
          panel['#attributes']['class'] += ' first';
          firstPanelId = this;
        }
        
        i++;
      }
    });
    if (element['#accordion']['active'] === null) {
      element['#accordion']['active'] = false;
    }
    if (i > 0) {
      i--;
      if (element['#accordion']['active'] !== false && element['#accordion']['active'] > i) {
        element['#accordion']['active'] = i;
        panel['#active_panel'] = true;
      }
    }
    if (typeof(panel['#attributes']) == 'undefined' || panel['#attributes'] === null) {
      panel['#attributes'] = {};
    }
    if (typeof(panel['#attributes']['class']) == 'undefined' || panel['#attributes']['class'] === null) {
      panel['#attributes']['class'] = '';
    }
    panel['#attributes']['class'] += ' last';
    
    if ((!element['#collapsible'] || !element['#collapsed']) && element['#accordion']['active'] === false) {
      element['#accordion']['active'] = 0;
      element[firstPanelId]['#active_panel'] = true;
    }
    
    if (typeof(Drupal.settings.gmaps.accordion) == 'undefined') {
      Drupal.settings.gmaps.accordion = {};
    }
    Drupal.settings.gmaps.accordion[element['#id']] = {'options': element['#accordion']};
    
    if (typeof(element['#attributes']) == 'undefined' || element['#attributes'] === null) {
      element['#attributes'] = {};
    }
    if (typeof(element['#attributes']['class']) == 'undefined' || element['#attributes']['class'] === null) {
      element['#attributes']['class'] = '';
    }
    var cls = 'form-gmaps-accordion';
    if (element['#embedded_item']) {
      cls += ' gmaps-accordion-embedded';
    }
    element['#attributes']['class'] = cls +' '+ element['#attributes']['class'];
    
    return element;
  };

  Drupal.hooks.elements.gmaps_accordion = {'#pre_render': {'default': preRender}};
})();

Drupal.gmaps.tabs = Drupal.gmaps.tabs || new (function() {
  var self = this;
  this.tabs = {};
  
  this.behaviors= {};
  
  this.attachBehaviors = function(t) {
    if (t) {
      $.each(self.behaviors, function() {
        this(t);
      });
    }
  };
  
  var beforeAhah = function(id, ui, o) {
    $(ui.tab).addClass('gmaps-tabs-loading');
    $(ui.panel).addClass('gmaps-tabs-content-loading');

    var wrapper = $(o.wrapper, ui.panel);
    
    // Insert progressbar or throbber.
    if (o.progress.type == 'bar') {
      var progressBar = new Drupal.progressBar('ahah-progress-' + id, eval(o.progress.update_callback), o.progress.method, eval(o.progress.error_callback));
      if (o.progress.message) {
        progressBar.setProgress(-1, o.progress.message);
      }
      if (o.progress.url) {
        progressBar.startMonitoring(o.progress.url, o.progress.interval || 1500);
      }
      o.progress.element = $(progressBar.element).addClass('ahah-progress ahah-progress-bar');
      o.progress.object = progressBar;
      wrapper.prepend(o.progress.element);
    }
    else if (o.progress.type == 'throbber') {
      o.progress.element = $('<div class="ahah-progress ahah-progress-throbber"><div class="throbber">&nbsp;</div></div>');
      $('.throbber', o.progress.element).after('<div class="clear-throbber""></div>')
      if (o.progress.message) {
        $('.throbber', o.progress.element).after('<div class="message">' + o.progress.message + '</div>')
      }
      wrapper.prepend(o.progress.element);
    }
  };
  
  var ahahSuccess = function(r, element, ui, o) {
    var wrapper = $(o.wrapper, ui.panel);

    // Restore the previous action and target to the form.
    if (o.form) {
      o.form.form.attr('action', o.form.form_action);
      o.form.form_target ? o.form.form.attr('target', o.form.form_target) : o.form.form.removeAttr('target');
      o.form.form_encattr ? o.form.form.attr('target', o.form.form_encattr) : o.form.form.removeAttr('encattr');
    }
    
    // Manually insert HTML into the jQuery object, using $() directly crashes
    // Safari with long string lengths. http://dev.jquery.com/ticket/1152
    if (o.ajax) {
      r = r.data;
    }
    var new_content = $("<div/>");
    new_content.html(o.selector ?
      new_content
        .append(r.replace(/<script(.|\s)*?\/script>/g, ""))
        .find(o.selector) :
      r);

    // Remove the progress element.
    if (o.progress.element) {
      $(o.progress.element).remove();
    }
    if (o.progress.object) {
      o.progress.object.stopMonitoring();
    }
    if (o.cache) {
      $(ui.panel).addClass('gmaps-tabs-ahah-processed');
    }
    $(ui.tab).removeClass('gmaps-tabs-loading');
    $(ui.panel).removeClass('gmaps-tabs-content-loading');

    // Add the new content to the page.
    Drupal.freezeHeight();
    if (o.method == 'replace') {
      wrapper.empty().append(new_content);
    }
    else {
      wrapper[o.method](new_content);
    }

    // Immediately hide the new content if we're using any effects.
    if (o.showEffect != 'show') {
      new_content.hide();
    }

    // Determine what effect use and what content will receive the effect, then
    // show the new content. For browser compatibility, Safari is excluded from
    // using effects on table rows.
    if (($.browser.safari && $("tr.ahah-new-content", new_content).size() > 0)) {
      new_content.show();
    }
    else if ($('.ahah-new-content', new_content).size() > 0) {
      $('.ahah-new-content', new_content).hide();
      new_content.show();
      $(".ahah-new-content", new_content)[o.showEffect](o.showSpeed);
    }
    else if (o.showEffect != 'show') {
      new_content[o.showEffect](o.showSpeed);
    }

    // Attach all javascript behaviors to the new content, if it was successfully
    // added to the page, this if statement allows #ahah[wrapper] to be optional.
    if (new_content.parents('html').length > 0) {
      Drupal.attachBehaviors(new_content);
    }

    Drupal.unfreezeHeight();
    
    $(element).trigger('gmapstabsload', [ui]);
  };
  
  var ahahError = function(r, element, ui, o) {
    alert(Drupal.ahahError(r, o.url));

    // Remove the progress element.
    if (o.progress.element) {
      $(o.progress.element).remove();
    }
    if (o.progress.object) {
      o.progress.object.stopMonitoring();
    }
    // Undo hide.
    $(o.wrapper).show();
    
    $(ui.tab).removeClass('gmaps-tabs-loading');
    $(ui.panel).removeClass('gmaps-tabs-content-loading');
  };
  
  this.getTabs = function(id, context, refresh) {
    if (self.tabs[id]) {
      if (refresh) {
        self.tabs[id] = null;
      }
      else {
        return self.tabs[id];
      }
    }
    context = context || document;
    var item = $('#'+ id, context), element = $('.gmaps-tabs:first', item), s = {options: {}};
    if (typeof(Drupal.settings.gmaps.tabs) != 'undefined' && typeof(Drupal.settings.gmaps.tabs[id]) != 'undefined') {
      s = $.extend(true, s, Drupal.settings.gmaps.tabs[id]);
    }
    if (typeof(s.options.selected) != 'undefined' && s.options.selected !== null) {
      s.options.selected = parseInt(s.options.selected);
    }
    if (typeof(s.options.disabled) != 'undefined') {
      var disabled = s.options.disabled;
      s.options.disabled = [];
      $.each(disabled, function(i, e) {if (e !== null) {s.options.disabled.push(parseInt(e, 10));}});
    }

    //AHAH/AJAX
    if (typeof(s.ahahPanels) != 'undefined') {
      var contentLoader = function(e, ui) {
        if (typeof(ui.panel) != 'undefined' && typeof(s.ahahPanels[ui.panel.id]) != 'undefined') {
          var ahahOpts = $.extend(true, {}, s.ahahOptions || {}, s.ahahPanels[ui.panel.id]);
          
          if (!ahahOpts.cache || !$(ui.panel).hasClass('gmaps-tabs-ahah-processed')) {
            Drupal.gmaps.expandAhahOptions(ahahOpts, item);
            beforeAhah(item[0].id, ui, ahahOpts);
            
            var ajax = {
              url: ahahOpts.url,
              type: 'POST',
              dataType: ahahOpts.ajax ? 'json' : 'html',
              cache: ahahOpts.cache,
              success: function(r, s) {
                // Sanity check for browser support (object expected).
                // When using iFrame uploads, responses must be returned as a string.
                if (ahahOpts.ajax && typeof(r) == 'string') {
                  r = Drupal.parseJson(r);
                }
                return ahahSuccess(r, element, ui, ahahOpts);
              },
              complete: function(r, s) {
                if (s == 'error' || s == 'parsererror') {
                  return ahahError(r, element, ui, ahahOpts);
                }
              }
            };
            ajax.data = {'#tabs': {id: id, panel_id: ui.panel.id}};
            if (ahahOpts.data) {
              ajax.data['#tabs'].data = ahahOpts.data;
            }
            if (!ahahOpts.ajax && ahahOpts.form) {
              $.each(ahahOpts.form.form.formToArray(), function() {
                ajax.data[this.name] = this.value;
              });
            }
            ajax.data = Drupal.gmaps.flatten(ajax.data);
            
            if (ahahOpts.ajax) {
              if (ahahOpts.form) {
                Drupal.gmaps.prepareAjaxOptions(ajax);
                ahahOpts.form.form.ajaxSubmit(ajax);
              }
              else {
                Drupal.gmaps.loadAjaxContent(ajax);
              }
            }
            else {
              $.ajax(ajax);
            }
          }
        }
      };
      s.options.show = contentLoader;
    }
    self.tabs[id] = element.tabs(s.options).addClass('gmaps-tabs-processed');

    var selected = false;
    $('.gmaps-tabs-panel', element).each(function(){
      if ($('.error', this).not('div.error').length) {
        $('a[href=#'+ this.id +']', element).addClass('error');
        if (!selected) {
          element.tabs('select', '#'+ this.id);
          selected = true;
        }
      }
    });
    
    self.attachBehaviors(element);
    
    return self.tabs[id];
  };
  
  this.cleanId = function(id) {
    var seenIds = $().data('seenIds.tabs.gmaps.drupal');
    if (!seenIds) {
      seenIds = {};
      $('.form-gmaps-tabs').each(function() {
        seenIds[this.id] = 1;
        $('.gmaps-tabs-panel', this).each(function() {
          seenIds[this.id] = 1;
        });
      });
    }

    var newid = id.replace(/[\]\[_\s]/g, '-');

    if (typeof(seenIds[newid]) != 'undefined') {
      newid = newid +'-'+ seenIds[newid]++;
    }
    else {
      seenIds[newid] = 1;
    }
    $().data('seenIds.tabs.gmaps.drupal', seenIds);

    return newid;
  };
  
  var preRender = function(element) {
    element['#id'] = self.cleanId('tabs-'+ (element['#id'] ? element['#id'] : 'gmaps'));
    
    element['#required'] = false;
    
    if (typeof(element['#nav_position']) == 'undefined' || element['#nav_position'] === null) {
      element['#nav_position'] = GMAPS_TABS_NAV_TOP;
    }
    
    if (typeof(element['#tabs']) == 'undefined' || element['#tabs'] === null) {
      element['#tabs'] = {};
    }
    
    if (typeof(element['#tabs']['selected']) == 'undefined' || !(!isNaN(element['#tabs']['selected']) || element['#tabs']['selected'] === null)) {
      element['#tabs']['selected'] = false;
    }
    
    element['#tabs']['tabSelector'] = 'li.'+ element['#id'] +'-tab';
    element['#tabs']['tabTemplate'] = '<li class="gmaps-tabs-tab '+ element['#id'] +'-tab"><a href="#{href}"><span>#{label}</span></a></li>';
    element['#tabs']['panelTemplate'] = '<div class="gmaps-tabs-panel '+ element['#id'] +'-panel"></div>';
    element['#tabs']['deselectableClass'] = 'ui-tabs-collapsible';
    element['#tabs']['idPrefix'] = element['#id'] +'-tab-';

    if (element['#nav_position'] == GMAPS_TABS_NAV_TOP) {
      if (typeof(element['#collapsible']) == 'undefined') {
        element['#collapsible'] = false;
      }
    }
    else {
      element['#collapsible'] = false;
    }
    element['#tabs']['collapsible'] = element['#tabs']['deselectable'] = element['#collapsible'];
    if (element['#collapsible']) {
      if (element['#collapsed']) {
        element['#tabs']['selected'] = null;
      }
      else if (element['#tabs']['selected'] === null || element['#tabs']['selected'] === false) {
        element['#tabs']['selected'] = 0;
      }
    }
    
    //tabs
    var i = 0, panel, firstPanelId;
    $.each(Drupal.elementChildren(element), function() {
      if (typeof(element[this]['#type']) != 'undefined' && element[this]['#type'] == 'gmaps_tabs_panel') {
        panel = element[this];
        panel['#id'] = self.cleanId(element['#id'] +'-'+ (panel['#id'] ? panel['#id'] : 'panel'));
        panel['#tabs_id'] = element['#id'];
        
        //synchronize CSS and JS
        if (panel['#selected_panel']) {
          if (element['#tabs']['selected'] === false) {
            element['#tabs']['selected'] = i;
          }
          else if (element['#tabs']['selected'] === null || element['#tabs']['selected'] != i) {
            panel['#selected_panel'] = false;
          }
        }
        else if (element['#tabs']['selected'] !== null && element['#tabs']['selected'] !== false && element['#tabs']['selected'] == i) {
          panel['#selected_panel'] = true;
        }
        
        if (i == 0) {
          if (typeof(panel['#attributes']) == 'undefined' || panel['#attributes'] === null) {
            panel['#attributes'] = {};
          }
          if (typeof(panel['#attributes']['class']) == 'undefined' || panel['#attributes']['class'] === null) {
            panel['#attributes']['class'] = '';
          }
          panel['#attributes']['class'] += ' first';
          firstPanelId = this;
        }
        
        i++;
      }
    });
    if (element['#tabs']['selected'] === false) {
      element['#tabs']['selected'] = null;
    }
    if (i > 0) {
      i--;
      if (element['#tabs']['selected'] !== null && element['#tabs']['selected'] > i) {
        element['#tabs']['selected'] = i;
        panel['#selected_panel'] = true;
      }
    }
    if (typeof(panel['#attributes']) == 'undefined' || panel['#attributes'] === null) {
      panel['#attributes'] = {};
    }
    if (typeof(panel['#attributes']['class']) == 'undefined' || panel['#attributes']['class'] === null) {
      panel['#attributes']['class'] = '';
    }
    panel['#attributes']['class'] += ' last';
    
    if ((!element['#collapsible'] || !element['#collapsed']) && element['#tabs']['selected'] === null) {
      element['#tabs']['selected'] = 0;
      element[firstPanelId]['#selected_panel'] = true;
    }
    
    if (typeof(Drupal.settings.gmaps.tabs) == 'undefined') {
      Drupal.settings.gmaps.tabs = {};
    }
    Drupal.settings.gmaps.tabs[element['#id']] = {'options': element['#tabs']};
    
    if (typeof(element['#attributes']) == 'undefined' || element['#attributes'] === null) {
      element['#attributes'] = {};
    }
    if (typeof(element['#attributes']['class']) == 'undefined' || element['#attributes']['class'] === null) {
      element['#attributes']['class'] = '';
    }
    var cls = 'form-gmaps-tabs gmaps-tabs-nav-'+ element['#nav_position'];
    if (element['#embedded_item']) {
      cls += ' gmaps-tabs-embedded';
    }
    element['#attributes']['class'] = cls +' '+ element['#attributes']['class'];
    
    return element;
  };

  Drupal.hooks.elements.gmaps_tabs = {'#pre_render': {'default': preRender}};
})();

/**
 * Cache object.
 */
GMapsCache = function() {
  var cache = {};
  
  this.set = function(cid, data) {
    cache[cid] = data;
  };
  
  this.get = function(cid) {
    if (typeof(cache[cid]) != 'undefined') {
      return cache[cid];
    }
    return null;
  };
  
  this.clearAll = function(cid) {
    if (cid == null) {
      cache = {};
    }
    else {
      delete cache[cid];
    }
  };
};

/**
 * Icon select object
 * 
 * @param select
 *  jQuery object of the "select" dom element
 */
GMapsIconSelectElement = function(select) {
  var gis = this;
  var parent = $(select).parent('div.gis-preview-wrapper');
  var img = $('img.gis-preview', parent);
  
  var preview = function(e) {
    img.hide();
    
    if (typeof(Drupal.settings.gmaps) == 'undefined' || typeof(Drupal.settings.gmaps.iconSelect) == 'undefined' ||
        typeof(Drupal.settings.gmaps.iconSelect[select[0].id]) == 'undefined') {
      return;
    }
    
    var attr = Drupal.settings.gmaps.iconSelect[select[0].id][select.fieldValue()[0]];
    
    if (typeof(attr) == 'undefined' || typeof(attr.src) == 'undefined') {
    }
    else {
      img.attr('src', attr.src);
      img.css({'width': attr.width, 'height': attr.height});
      img.show();
    }
  };
  
  select.change(preview).keyup(preview);
  
  preview();
};


