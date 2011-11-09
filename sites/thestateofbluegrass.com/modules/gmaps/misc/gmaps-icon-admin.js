// $Id: gmaps-icon-admin.js,v 1.1.2.4 2010/03/01 11:51:10 xmarket Exp $

Drupal.behaviors.gmapsIconColorForm = function (context) {
  if ($('#gmaps-icon-color-form .color-form.color-processed').size()) {
    return;
  }
  var form = $('#gmaps-icon-color-form .color-form', context);
  var inputs = [];
  var hooks = [];
  var locks = [];
  var focused = null;

  // Add Farbtastic
  $(form).prepend('<div id="placeholder"></div>').addClass('color-processed');;
  var farb = $.farbtastic('#placeholder');

  // Decode reference colors to HSL
  var reference = Drupal.settings.gmapsIconColorForm.reference;
  for (i in reference) {
    reference[i] = farb.RGBToHSL(farb.unpack(reference[i]));
  }

  /**
   * Render the preview.
   */
  function preview() {
    if (typeof(Drupal.settings.gmapsIconColorForm.preview) != 'undefined') {
      var opts = eval(Drupal.settings.gmapsIconColorForm.preview + '();');
      var icon = Drupal.gmaps.icon.createIcon(opts);
      if (icon && icon.image && icon.image.length) {
        $('#gmaps-icon-preview').attr('src', icon.image);
        $('#gmaps-icon-preview').show();
      }
      else {
        $('#gmaps-icon-preview').hide();
      }
    }
  }

  function previewMarker() {
    var opts = {'data': {'cache': 0}, 'type': 'marker'};
    var iconSize = $('#edit-data-icon-size').get(0).value.split('x');
    opts.icon_size = {'width': iconSize[0], 'height': iconSize[1]};
    opts.data.primary_color = $('#edit-data-color-primary-color').get(0).value;
    opts.data.stroke_color = $('#edit-data-color-stroke-color').get(0).value;
    opts.data.corner_color = $('#edit-data-color-corner-color').get(0).value;
    
    return opts;
  }

  function previewLabeledMarker() {
    var opts = {'data': {'cache': 0}, 'type': 'labeled_marker'};
    opts.label = getLabel();
    opts.data.pin_style = $('#edit-data-pin-style').get(0).value;
    opts.data.label_cut = $('#edit-data-label-cut').get(0).value;
    opts.data.primary_color = $('#edit-data-color-primary-color').get(0).value;
    opts.data.stroke_color = $('#edit-data-color-stroke-color').get(0).value;
    opts.data.label_color = $('#edit-data-color-label-color').get(0).value;
    opts.data.star_primary_color = $('#edit-data-color-star-primary-color').get(0).value;
    opts.data.star_stroke_color = $('#edit-data-color-star-stroke-color').get(0).value;
    
    return opts;
  }

  function previewFlat() {
    var opts = {'data': {'cache': 0}, 'type': 'flat'};
    var iconSize = $('#edit-data-icon-size').get(0).value.split('x');
    opts.icon_size = {'width': iconSize[0], 'height': iconSize[1]};
    opts.label = getLabel();
    opts.data.label_size = $('#edit-data-label-size').get(0).value;
    opts.data.shape = $('#edit-data-shape').get(0).value;
    opts.data.shadow_from_transparency = $('#edit-data-shadow-from-transparency').get(0).value;
    opts.data.shadow_to_transparency = $('#edit-data-shadow-to-transparency').get(0).value;
    opts.data.label_color = $('#edit-data-color-label-color').get(0).value;
    opts.data.primary_color = $('#edit-data-color-primary-color').get(0).value;
    opts.data.shadow_from_color = $('#edit-data-color-shadow-from-color').get(0).value;
    opts.data.shadow_to_color = $('#edit-data-color-shadow-to-color').get(0).value;
    
    return opts;
  }

  function previewIconicMarker() {
    var opts = {'data': {'cache': 0}, 'type': 'iconic_marker'};
    opts.data.pin_style = $('#edit-data-pin-style').get(0).value;
    opts.data.pin_icon = $('#edit-data-pin-icon').get(0).value;
    opts.data.primary_color = $('#edit-data-color-primary-color').get(0).value;
    opts.data.star_color = $('#edit-data-color-star-color').get(0).value;
    
    return opts;
  }

  function previewScaledMarker() {
    var opts = {'data': {'cache': 0}, 'type': 'scaled_marker'};
    opts.label = getLabel();
    opts.data.scale = $('#edit-data-scale').get(0).value;
    opts.data.rotation = $('#edit-data-rotation').get(0).value;
    opts.data.label_size = $('#edit-data-label-size').get(0).value;
    opts.data.label_style = $('#edit-data-label-style').get(0).value;
    opts.data.primary_color = $('#edit-data-color-primary-color').get(0).value;
    
    return opts;
  }
  
  function getLabel() {
    var label = $('#edit-label');
    if (label[0].tagName == 'INPUT') {
      return label.val();
    }
    else {
      return label.html();
    }
  }

  function shift_color(given, ref1, ref2) {
    // Convert to HSL
    given = farb.RGBToHSL(farb.unpack(given));

    // Hue: apply delta
    given[0] += ref2[0] - ref1[0];

    // Saturation: interpolate
    if (ref1[1] == 0 || ref2[1] == 0) {
      given[1] = ref2[1];
    }
    else {
      var d = ref1[1] / ref2[1];
      if (d > 1) {
        given[1] /= d;
      }
      else {
        given[1] = 1 - (1 - given[1]) * d;
      }
    }

    // Luminance: interpolate
    if (ref1[2] == 0 || ref2[2] == 0) {
      given[2] = ref2[2];
    }
    else {
      var d = ref1[2] / ref2[2];
      if (d > 1) {
        given[2] /= d;
      }
      else {
        given[2] = 1 - (1 - given[2]) * d;
      }
    }

    return farb.pack(farb.HSLToRGB(given));
  }

  /**
   * Callback for Farbtastic when a new color is chosen.
   */
  function callback(input, color, propagate, colorscheme) {
    // Set background/foreground color
    $(input).css({
      'backgroundColor': color,
      'color': farb.RGBToHSL(farb.unpack(color))[2] > 0.5 ? '#000' : '#fff'
    });

    // Change input value
    if (input.value && input.value != color) {
      input.value = color;

      // Update locked values
      if (propagate) {
        var i = input.i;
        for (var j = i + 1; ; ++j) {
          if (!locks[j - 1] || $(locks[j - 1]).is('.unlocked')) break;
          var matched = shift_color(color, reference[input.key], reference[inputs[j].key]);
          callback(inputs[j], matched, false);
        }
        for (var j = i - 1; ; --j) {
          if (!locks[j] || $(locks[j]).is('.unlocked')) break;
          var matched = shift_color(color, reference[input.key], reference[inputs[j].key]);
          callback(inputs[j], matched, false);
        }

        // Update preview
        preview();
      }
    }
  }

  // Focus the Farbtastic on a particular field.
  function focus() {
    var input = this;
    // Remove old bindings
    focused && $(focused).unbind('keyup', farb.updateValue)
      .unbind('keyup', preview).parent().removeClass('item-selected');

    // Add new bindings
    focused = this;
    farb.linkTo(function (color) { callback(input, color, true, false); });
    farb.setColor(this.value);
    $(focused).keyup(farb.updateValue).keyup(preview).parent().addClass('item-selected');
  }

  // Initialize color fields
  $('#palette input.form-text', form)
  .each(function () {
    // Extract palette field name
    this.key = this.id.substring(16);

    // Link to color picker temporarily to initialize.
    farb.linkTo(function () {}).setColor('#000').linkTo(this);

    // Add lock
    var i = inputs.length;
    if (inputs.length) {
      var lock = $('<div class="lock"></div>').toggle(
        function () {
          $(this).addClass('unlocked');
          $(hooks[i - 1]).attr('class',
            locks[i - 2] && $(locks[i - 2]).is(':not(.unlocked)') ? 'hook up' : 'hook'
          );
          $(hooks[i]).attr('class',
            locks[i] && $(locks[i]).is(':not(.unlocked)') ? 'hook down' : 'hook'
          );
        },
        function () {
          $(this).removeClass('unlocked');
          $(hooks[i - 1]).attr('class',
            locks[i - 2] && $(locks[i - 2]).is(':not(.unlocked)') ? 'hook both' : 'hook down'
          );
          $(hooks[i]).attr('class',
            locks[i] && $(locks[i]).is(':not(.unlocked)') ? 'hook both' : 'hook up'
          );
        }
      );
      $(this).after(lock);
      locks.push(lock);
    }

    // Add hook
    var hook = $('<div class="hook"></div>');
    $(this).after(hook);
    hooks.push(hook);

    $(this).parent().find('.lock').click();
    this.i = i;
    inputs.push(this);
  })
  .focus(focus);
  
  if (typeof(Drupal.settings.gmapsIconColorForm.init) != 'undefined') {
    eval(Drupal.settings.gmapsIconColorForm.init + '();');
  }
  
  function initMarker() {
    $('#edit-data-icon-size').keyup(preview);
  };

  function initLabeledMarker() {
    $('#edit-label').keyup(preview);
    $('#edit-data-pin-style').change(preview);
    $('#edit-data-pin-style').keyup(preview);
    $('#edit-data-label-cut').change(preview);
    $('#edit-data-label-cut').keyup(preview);
  };

  function initFlat() {
    $('#edit-label').keyup(preview);
    $('#edit-data-label-size').keyup(preview);
    $('#edit-data-icon-size').keyup(preview);
    $('#edit-data-shape').change(preview);
    $('#edit-data-shape').keyup(preview);
    $('#edit-data-shadow-from-transparency').keyup(preview);
    $('#edit-data-shadow-to-transparency').keyup(preview);
  };

  function initIconicMarker() {
    $('#edit-data-pin-style').change(preview);
    $('#edit-data-pin-style').keyup(preview);
    $('#edit-data-pin-icon').change(preview);
    $('#edit-data-pin-icon').keyup(preview);
  };

  function initScaledMarker() {
    $('#edit-label').keyup(preview);
    $('#edit-data-scale').keyup(preview);
    $('#edit-data-rotation').keyup(preview);
    $('#edit-data-label-size').keyup(preview);
    $('#edit-data-label-style').change(preview);
    $('#edit-data-label-style').keyup(preview);
  };

  $('#palette label', form);

  // Focus first color
  focus.call(inputs[0]);

  // Render preview
  preview();
}
