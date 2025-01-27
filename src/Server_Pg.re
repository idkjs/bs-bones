// Hint, fold the code for readability, yeah?
type pool = {
  .
  "totalCount": int,
  "idleCount": int,
  "waitingCount": int,
};
type dbResult = {. "rows": Js.Json.t};
type values;
type queryPayload = {
  .
  "text": string,
  "values": values,
};

module Pool = {
  [@bs.module "pg"] [@bs.new]
  external make: {. "connectionString": string} => pool = "Pool";

  [@bs.send]
  external _query: (pool, queryPayload) => Js.Promise.t(dbResult) = "query";

  let query = (pool, payload) => _query(pool, payload);
};

module Client = {
  type t;

  [@bs.send] external connect: pool => Js.Promise.t(t) = "connect";

  [@bs.send]
  external _query: (t, queryPayload) => Js.Promise.t(dbResult) = "query";
  let query = (client, payload) => _query(client, payload);

  [@bs.send] external release: t => unit = "release";
};

module Events = {
  [@bs.send]
  external onConnect: (pool, [@bs.as "connect"] _, Client.t => unit) => unit =
    "on";

  [@bs.send]
  external onAcquire: (pool, [@bs.as "acquire"] _, Client.t => unit) => unit =
    "on";

  [@bs.send]
  external onError:
    (pool, [@bs.as "error"] _, (Js.Exn.t, Client.t) => unit) => unit =
    "on";

  [@bs.send]
  external onRemove: (pool, [@bs.as "remove"] _, Client.t => unit) => unit =
    "on";

  let poolStats = (pool: pool) =>
    "totalCount="
    ++ string_of_int(pool##totalCount)
    ++ ",idleCount="
    ++ string_of_int(pool##idleCount)
    ++ ",waitingCount="
    ++ string_of_int(pool##waitingCount)
    ++ ",";
};

module DbTypes = {
  type t;

  [@bs.val] external parseInt: string => int = "parseInt";
  [@bs.module "pg"] external types: t = "types";
  [@bs.send] external setTypeParser: (t, int, string => 'a) => unit = "setTypeParser";
  [@bs.send]
  external setTypeParserD: (t, int, Js.Date.t => 'a) => unit = "setTypeParser";

  let parseDate = (a: Js.Date.t) => {
    Js.Date.toString(a);
  };

  types->setTypeParser(20, parseInt);
  types->setTypeParserD(1184, parseDate);
};
