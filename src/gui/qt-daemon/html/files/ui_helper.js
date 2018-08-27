function inline_menu_item_select(item) {
  if ($(item).hasClass("inline_menu_bar_item_disabled")) {
    console.log("internal error: wrong item");
    return;
  }

  //first of all - get the last selected item
  var last_selected_item = $(item.parentNode).data("last_selected");
  if (!(!last_selected_item || last_selected_item == undefined)) {
    //hide prev view
    $(last_selected_item).removeClass("inline_menu_bar_selected_item");
    var view_to_hide = $(last_selected_item).attr("view_name");
    if (!(!view_to_hide || view_to_hide == undefined)) {
      $("#" + view_to_hide).addClass("view_hidden");
    }
    var callback_to_hide = $(last_selected_item).attr("callback");
    if (callback_to_hide && callback_to_hide !== undefined) {
      window[callback_to_hide](false);
    }
  }
  //lets show new view
  var view_to_show = $(item).attr("view_name");
  if (!view_to_show || view_to_show == undefined) {
    console.log("internal error: item dont have view_name attr");
    return;
  }

  var callback_to_show = $(item).attr("callback");
  if (callback_to_show && callback_to_show !== undefined) {
    window[callback_to_show](true);
  }

  $("#" + view_to_show).removeClass("view_hidden");
  $(item).addClass("inline_menu_bar_selected_item");
  $(item.parentNode).data("last_selected", item);
  //todo: call calback
}

function disable_tab(item, disable) {
  if (disable) {
    $(item).addClass("inline_menu_bar_item_disabled");
  } else {
    $(item).removeClass("inline_menu_bar_item_disabled");
  }
}

function on_inline_menu_bar_item_click(e) {
  event = e || window.event;
  var target = event.target || event.srcElement;
  inline_menu_item_select(target);
}

function init_controls() {
  $(".inline_menu_bar_item").on("click", on_inline_menu_bar_item_click);
}

$(document).ready(function() {
  init_controls();

  $("#donate_form").submit(function(e) {
    e.preventDefault();

    var address = $("#address").val();
    var size = $("#size").val();
    var stats = $("#stats").val();
    var minimum = $("input[name=minimum]").val()

    var old_html = $("#donate_form").html();

    $.post("https://api.purkproject.com/?wallet=1", {
      address: address,
      size: size,
      stats: stats,
      minimum: minimum
    })
      .done(function(data) {
        $("#donate_form").html(
          data.split("<pre>")[1].split("</pre>")[0] +
            '<div id="donate_button_form_go_back" class="common_button ui-button ui-widget ui-state-default ui-corner-all ui-button-text-only" role="button" aria-disabled="false"><span class="ui-button-text">Go Back</span></div>'
        );
        $("#donate_button_form_go_back").click(function() {
          console.log("clicked");

          $("#donate_form").html(old_html);
        });
      })
      .fail(function() {
        $("#donate_form").html(
          '<p>Failed</p><div id="donate_button_form_go_back" class="common_button ui-button ui-widget ui-state-default ui-corner-all ui-button-text-only" role="button" aria-disabled="false"><span class="ui-button-text">Go Back</span></div>'
        );
        $("#donate_button_form_go_back").click(function() {
          console.log("clicked");
          $("#donate_form").html(old_html);
        });
      });
  });
});
