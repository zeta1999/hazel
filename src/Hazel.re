module Js = Js_of_ocaml.Js;
module Dom = Js_of_ocaml.Dom;
module Dom_html = Js_of_ocaml.Dom_html;
open Incr_dom;

// https://github.com/janestreet/incr_dom/blob/6aa4aca2cfc82a17bbcc0424ff6b0ae3d6d8d540/example/text_input/README.md
// https://github.com/janestreet/incr_dom/blob/master/src/app_intf.ml

module Model = Model;
module Action = Update.Action;
module State = {
  type t = unit;
};

[@warning "-27"]
let on_startup = (~schedule_action, _) => {
  let _ =
    JSUtil.listen_to_t(
      Dom.Event.make("selectionchange"), Dom_html.document, _ =>
      schedule_action(Update.Action.SelectionChange)
    );
  Async_kernel.Deferred.return();
};

[@warning "-27"]
let create = (model, ~old_model, ~inject) => {
  open Incr.Let_syntax;
  let%map model = model;
  Component.create(
    ~apply_action=Update.apply_action(model),
    ~on_display=
      (_, ~schedule_action) => {
        let path = model |> Model.path;
        if (model.is_cell_focused && !Code.is_caret_consistent_with_path(path)) {
          switch (Code.caret_position_of_path(path)) {
          | None => raise(ViewUtil.MalformedView(10))
          | Some((node, offset)) => JSUtil.set_caret(node, offset)
          };
        };
      },
    model,
    Page.view(~inject, model),
  );
};
