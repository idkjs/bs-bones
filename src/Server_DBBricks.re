// This is organized into a bunch of modules that are each "included"
// at the top level so that if "Bricks" is opened, the user has access
// to everything. But other modules can choose to "include" just some
// subset of functionality.
module Types = {
  type builder;
  type insertBuilder;
  type fragment = {
    str: string,
    vals: Server_Pg.values,
  };
  type onConflictBuilder;
};
include Types;

module Utils = {
  /* Remove any undefined values from an object that we are about to set */
  [@bs.module "lodash"]
  external omitBy: (Js.t('a), 'b => bool) => Js.t('c) = "omitBy";
  [@bs.module "lodash"]
  external omitByJson: (Js.Json.t, 'b => bool) => Js.Json.t = "omitBy";
};

module Raw = {
  [@bs.module] external sql: string => fragment = "sql-bricks-postgres";

  [@bs.module]
  external sqlWithValues:
    (
      ~text: string,
      ~values: [@bs.unwrap] [
                 | `One('a)
                 | `Two('a, 'b)
                 | `Three('a, 'b, 'c)
                 | `Four('a, 'b, 'c, 'd)
                 | `Five('a, 'b, 'c, 'd, 'e)
                 | `Six('a, 'b, 'c, 'd, 'e, 'f)
                 | `Seven('a, 'b, 'c, 'd, 'e, 'f, 'g)
                 | `Eight('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h)
                 | `Nine('a, 'b, 'c, 'd, 'e, 'f, 'g, 'h, 'i)
               ]
    ) =>
    fragment =
    "sql-bricks-postgres";
};
include Raw;

module Where = {
  type whereExpr;
  [@bs.module "sql-bricks-postgres"]
  external and_: array(whereExpr) => whereExpr = "and";
  [@bs.module "sql-bricks-postgres"]
  external or_: array(whereExpr) => whereExpr = "or";
  [@bs.module "sql-bricks-postgres"] external not: whereExpr => whereExpr = "not";
  [@bs.module "sql-bricks-postgres"]
  external eq: (~col: string, 'a) => whereExpr = "eq";
  [@bs.module "sql-bricks-postgres"]
  external notEq: (~col: string, 'a) => whereExpr = "notEq";
  [@bs.module "sql-bricks-postgres"]
  external lt: (~col: string, 'a) => whereExpr = "lt";
  [@bs.module "sql-bricks-postgres"]
  external lte: (~col: string, 'a) => whereExpr = "lte";
  [@bs.module "sql-bricks-postgres"]
  external gt: (~col: string, 'a) => whereExpr = "gt";
  [@bs.module "sql-bricks-postgres"]
  external gte: (~col: string, 'a) => whereExpr = "gte";
  [@bs.module "sql-bricks-postgres"]
  external between: (~col: string, ~bottom: 'a, ~top: 'a) => whereExpr = "between";
  [@bs.module "sql-bricks-postgres"] external isNull: string => whereExpr = "isNull";
  [@bs.module "sql-bricks-postgres"]
  external isNotNull: string => whereExpr = "isNotNull";
  [@bs.module "sql-bricks-postgres"]
  external like: (~col: string, string) => whereExpr = "like";
  [@bs.module "sql-bricks-postgres"]
  external exists: (~subquery: builder) => whereExpr = "exists";
  [@bs.module "sql-bricks-postgres"]
  external in_: (~col: string, array('a)) => whereExpr = "in";
  [@bs.module "sql-bricks-postgres"]
  external isNot: whereExpr => whereExpr = "not";
};
include Where;

module Insert = {
  [@bs.module "sql-bricks-postgres"]
  external insertInto: string => insertBuilder = "insertInto";
  [@bs.send] external into: (builder, string) => builder = "into";
  [@bs.send] external intoTemp: (builder, string) => builder = "intoTemp";
  [@bs.send]
  external values: (insertBuilder, array(Js.t('a))) => builder = "values";
  [@bs.send]
  external valuesJson: (insertBuilder, array(Js.Json.t)) => builder =
    "values";
  [@bs.send] external value: (insertBuilder, Js.t('a)) => builder = "values";
  let value = (builder, v) =>
    value(builder, Utils.omitBy(v, Js.Undefined.testAny));
  [@bs.send] external anyValue: (insertBuilder, 'a) => builder = "values";
  let anyValue = (builder, v) => anyValue(builder, v);
  [@bs.send]
  external valueJson: (insertBuilder, Js.Json.t) => builder = "values";
  let valueJson = (builder, v) =>
    valueJson(builder, Utils.omitByJson(v, Js.Undefined.testAny));
};
include Insert;

module Update = {
  /* The default behavior of the library is to set properties to "null" in the database whose keys are defined,
     and whose values are undefined. But the behavior we want is that when the value of an attribute is undefined,
     the key is left alone in the database. On the other hand, if the value is "null", then the value should
     be set to null in the database. */
  [@bs.module "sql-bricks-postgres"]
  external update: (~table: string, Js.t('a)) => builder = "update";
  let update = (~table, obj) =>
    update(~table, Utils.omitBy(obj, Js.Undefined.testAny));
  [@bs.module "sql-bricks-postgres"]
  external updateTableOnly: string => builder = "updateTableOnly";
  [@bs.module "sql-bricks-postgres"]
  external updateJson: (~table: string, Js.Json.t) => builder = "update";
  let updateJson = (~table, json) =>
    updateJson(~table, Utils.omitByJson(json, Js.Undefined.testAny));
  [@bs.send] external set: (builder, ~column: string, 'a) => builder = "set";
};
include Update;

module Delete = {
  [@bs.module "sql-bricks-postgres"] external delete: unit => builder = "delete";
};
include Delete;

module Select = {
  [@bs.module "sql-bricks-postgres"] external select: string => builder = "select";
  [@bs.send] external from: (builder, string) => builder = "from";
  [@bs.send] external forUpdate: (builder, unit) => builder = "forUpdate";
};
include Select;

module Filters = {
  [@bs.send]
  external distinct: (builder, ~cols: array(string)) => builder = "distinct";
  [@bs.send] external having: (builder, ~column: string, 'a) => builder = "having";
  [@bs.send] external limit: (builder, int) => builder = "limit";
  [@bs.send] external offset: (builder, int) => builder = "offset";
  [@bs.send] external whereEq: (builder, string, string) => builder = "where";
  [@bs.send] external whereAll: (builder, Js.t('a)) => builder = "where";
  [@bs.send] external where: (builder, whereExpr) => builder = "where";
  let whereOpt = (builder: builder, optExpr: option(whereExpr)): builder => {
    switch (optExpr) {
    | None => builder
    | Some(expr) => where(builder, expr)
    };
  };
};
include Filters;

module Transformers = {
  [@bs.send] external andSelect: (builder, string) => builder = "select";
  [@bs.send] external join: (builder, string) => builder = "join";
  [@bs.send] external leftJoin: (builder, string) => builder = "leftJoin";
  [@bs.send] external rightJoin: (builder, string) => builder = "rightJoin";
  [@bs.send] external fullJoin: (builder, string) => builder = "fullJoin";
  [@bs.send] external crossJoin: (builder, string) => builder = "crossJoin";
  [@bs.send] external on: (builder, Js.t('a)) => builder = "on";
  [@bs.send] external groupByOne: (builder, string) => builder = "groupBy";
  [@bs.send] external groupBy: (builder, array(string)) => builder = "groupBy";
  [@bs.send] external orderByOne: (builder, string) => builder = "orderBy";
  [@bs.send] external orderBy: (builder, array(string)) => builder = "orderBy";
  [@bs.send] external returning: (builder, string) => builder = "returning";
};
include Transformers;

module OnConflict = {
  [@bs.send]
  external onConflict2: (builder, string, string) => onConflictBuilder =
    "onConflict";
  [@bs.send] external onConflict: (builder, string) => onConflictBuilder = "onConflict";
  [@bs.send] external doUpdate: onConflictBuilder => builder = "doUpdate";
  [@bs.send] external doNothing: onConflictBuilder => builder = "doNothing";
  [@bs.send]
  external doUpdateOne: (onConflictBuilder, string) => builder = "doUpdate";
};
include OnConflict;

module To = {
  [@bs.send]
  external toPayload: builder => Server_Pg.queryPayload = "toParams";
  [@bs.send] external toString: builder => string = "toString";
};
include To;
