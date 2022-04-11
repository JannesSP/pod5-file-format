#include "mkr_format/file_reader.h"
#include "mkr_format/file_writer.h"
#include "mkr_format/read_table_reader.h"
#include "mkr_format/signal_table_reader.h"
#include "utils.h"

#include <arrow/array/array_binary.h>
#include <arrow/array/array_primitive.h>
#include <arrow/memory_pool.h>
#include <boost/uuid/random_generator.hpp>
#include <catch2/catch.hpp>

SCENARIO("File Reader Writer Tests") {
    auto types = mkr::register_extension_types();

    auto split_file_signal = "./foo_signal.mkr";
    auto split_file_reads = "./foo_reads.mkr";

    auto const run_info_data = get_test_run_info_data("_run_info");
    auto const end_reason_data = get_test_end_reason_data();
    auto const pore_data = get_test_pore_data();
    auto const calibration_data = get_test_calibration_data();

    auto uuid_gen = boost::uuids::random_generator_mt19937();
    auto read_id_1 = uuid_gen();

    std::uint32_t read_number = 1234;
    std::uint64_t start_sample = 1234;
    float median_before = 224.0f;

    std::vector<std::int16_t> signal_1(100'000);
    std::iota(signal_1.begin(), signal_1.end(), 0);

    // Write a file:
    {
        mkr::FileWriterOptions options;

        auto writer = mkr::create_split_file_writer(split_file_signal, split_file_reads,
                                                    "test_software", options);
        REQUIRE(writer.ok());

        auto run_info = (*writer)->add_run_info(run_info_data);
        auto end_reason = (*writer)->add_end_reason(end_reason_data);
        auto pore = (*writer)->add_pore(pore_data);
        auto calibration = (*writer)->add_calibration(calibration_data);

        (*writer)->add_complete_read({read_id_1, *pore, *calibration, read_number, start_sample,
                                      median_before, *end_reason, *run_info},
                                     gsl::make_span(signal_1));
    }

    // Open the file for reading:
    // Write a file:
    {
        mkr::FileReaderOptions options;

        auto reader = mkr::open_split_file_reader(split_file_signal, split_file_reads, options);
        CAPTURE(reader);
        REQUIRE(reader.ok());

        REQUIRE((*reader)->num_read_record_batches() == 1);
        auto read_batch = (*reader)->read_read_record_batch(0);
        CHECK(read_batch.ok());

        auto read_id_array = read_batch->read_id_column();
        CHECK(read_id_array->Value(0) == read_id_1);

        REQUIRE((*reader)->num_signal_record_batches() == 1);
        auto signal_batch = (*reader)->read_signal_record_batch(0);
        CHECK(signal_batch.ok());

        auto signal_read_id_array = signal_batch->read_id_column();
        CHECK(signal_read_id_array->length() == 5);
        CHECK(signal_read_id_array->Value(0) == read_id_1);
        CHECK(signal_read_id_array->Value(1) == read_id_1);
        CHECK(signal_read_id_array->Value(2) == read_id_1);
        CHECK(signal_read_id_array->Value(3) == read_id_1);
        CHECK(signal_read_id_array->Value(4) == read_id_1);

        auto vbz_signal_array = signal_batch->vbz_signal_column();
        CHECK(vbz_signal_array->length() == 5);

        auto samples_array = signal_batch->samples_column();
        CHECK(samples_array->Value(0) == 20'480);
        CHECK(samples_array->Value(1) == 20'480);
        CHECK(samples_array->Value(2) == 20'480);
        CHECK(samples_array->Value(3) == 20'480);
        CHECK(samples_array->Value(4) == 18'080);
    }
}