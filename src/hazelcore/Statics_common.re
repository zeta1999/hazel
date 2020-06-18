[@deriving sexp]
type edit_state = (ZExp.t, HTyp.t, MetaVarGen.t);

type type_mode =
  | Syn
  | Ana(HTyp.t);

let tuple_zip =
    (
      ~get_tuple_elements: Skel.t('op) => list(Skel.t('op)),
      skel: Skel.t('op),
      ty: HTyp.t,
    )
    : option(list((Skel.t('op), HTyp.t))) => {
  let skels = skel |> get_tuple_elements;
  let tys = ty |> HTyp.get_prod_elements;
  switch (ListUtil.opt_zip(skels, tys)) {
  | Some(_) as zipped => zipped
  | None =>
    switch (skels, tys) {
    | ([_], _) => Some([(skel, ty)])
    | (_, [Hole]) =>
      skels |> List.map(skel => (skel, HTyp.Hole)) |> Option.some
    | _ => None
    }
  };
};

type syn_fixer('term, 'extra_input, 'extra_output) =
  (
    Contexts.t,
    MetaVarGen.t,
    ~renumber_empty_holes: bool,
    ~extra_input: 'extra_input,
    'term
  ) =>
  ('term, 'extra_output, MetaVarGen.t);
type ana_fixer('term, 'extra_input, 'extra_output) =
  (
    Contexts.t,
    MetaVarGen.t,
    ~renumber_empty_holes: bool,
    ~extra_input: 'extra_input,
    'term,
    HTyp.t
  ) =>
  ('term, 'extra_output, MetaVarGen.t);

// Wraps a syn_fixer to check if u_gen has changed. If it hasn't, then the value
// didn't change and we can ensure pointer stability by returning the original
// term.
let stable_syn_fixer =
    (f: syn_fixer('term, 'extra_input, 'extra_output))
    : syn_fixer('term, 'extra_input, 'extra_output) =>
  (ctx, u_gen, ~renumber_empty_holes, ~extra_input, term) => {
    let (fixed_term, extra_output, u_gen2) =
      f(ctx, u_gen, ~renumber_empty_holes, ~extra_input, term);
    if (u_gen2 == u_gen) {
      (term, extra_output, u_gen2);
    } else {
      (fixed_term, extra_output, u_gen2);
    };
  };
let stable_ana_fixer =
    (f: ana_fixer('term, 'extra_input, 'extra_output))
    : ana_fixer('term, 'extra_input, 'extra_output) =>
  (ctx, u_gen, ~renumber_empty_holes, ~extra_input, term, ty) => {
    let (fixed_term, extra_output, u_gen2) =
      f(ctx, u_gen, ~renumber_empty_holes, ~extra_input, term, ty);
    if (u_gen2 == u_gen) {
      (term, extra_output, u_gen2);
    } else {
      (fixed_term, extra_output, u_gen2);
    };
  };