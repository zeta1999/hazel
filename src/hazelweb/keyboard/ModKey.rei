module Js = Js_of_ocaml.Js;
module Dom_html = Js_of_ocaml.Dom_html;

type t =
  | Ctrl
  | Shift
  | Alt
  | Meta;

let matches: (t, Js.t(Dom_html.keyboardEvent)) => bool;
