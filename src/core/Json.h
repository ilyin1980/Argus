/**
 * @file Json.h
 * @brief Serialisation of results into the shape the CLI and the worker share.
 *
 * The CLI writes newline-delimited JSON so that a consumer can stream results
 * without waiting for the run to finish, and so an agent can parse line by
 * line. The same object shapes will be the HTTP response bodies in P1.
 */
#pragma once

#include <QJsonObject>
#include <QString>

#include "core/Types.h"

namespace iw {

/**
 * @brief Serialise one file row.
 * @param row  Row to serialise.
 * @param root Indexed root; when given, an absolute @c path field is added.
 * @return Object with @c rel, @c size, @c width, @c height, @c mtime and,
 *         where @p root allows it, @c path.
 * @note Consumers that pipe results into other tools want the absolute path;
 *       consumers that move an index between machines want the relative one.
 *       Emitting both costs nothing and spares everyone a join.
 */
QJsonObject toJson(const FileInfoRow &row, const QString &root = {});

/**
 * @brief Serialise one query hit.
 * @param match Hit to serialise.
 * @param root  Indexed root; when given, an absolute @c path field is added.
 * @return Object with @c type="match", the file fields, @c distance and @c score.
 */
QJsonObject toJson(const Match &match, const QString &root = {});

/**
 * @brief Serialise one duplicate group.
 * @param group Group to serialise.
 * @param root  Indexed root; when given, each file gains an absolute @c path.
 * @return Object with @c type="group", @c kind, @c files, @c wasted, @c max_distance.
 */
QJsonObject toJson(const DuplicateGroup &group, const QString &root = {});

/**
 * @brief Serialise indexing counters.
 * @param stats Counters to serialise.
 * @return Object with @c type="index_summary".
 */
QJsonObject toJson(const IndexStats &stats);

/**
 * @brief Serialise the non-group part of a duplicate report.
 * @param report Report to summarise.
 * @return Object with @c type="dupes_summary".
 */
QJsonObject summaryJson(const DuplicateReport &report);

/**
 * @brief Render an object as one NDJSON line.
 * @param object Object to render.
 * @return Compact JSON text without a trailing newline.
 */
QString toLine(const QJsonObject &object);

} // namespace iw
