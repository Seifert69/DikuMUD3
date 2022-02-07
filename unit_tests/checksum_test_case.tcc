/**
 * if source file doesn't exist on disk
 *      - Pass test: Remove source file from dataset
 *
 * if output file doesn't exist on disk
 *      - Fail test: Possible compiler error on compiling source
 *
 * If the source file hash has changed and the output files have not
 *      - Pass test: Update source file hash in dataset
 *
 * if the source file checksum and the output file(s) hash(es) have also changed
 *      - Pass test: Update source and output file hash in dataset
 *
 * if output file(s) hash(es) have changed but the source file hash has not
 *      - Fail test: (aka compiler possible breaking change)
 *
 */
BOOST_DATA_TEST_CASE(TestFileChecksums, config_files, filename)
{
    ChecksumDataset dataset(filename);

    for (auto it = dataset.begin(); it != dataset.end(); /* */)
    {
        FileInfo &file_info = it->second;

        auto source_file_filename = file_info.source_file.filename;
        BOOST_TEST_CONTEXT("Checking file:" << source_file_filename.string())
        {
            auto source_file_exists = std::filesystem::exists(source_file_filename);
            BOOST_WARN(source_file_exists);
            if (!source_file_exists)
            {
                BOOST_WARN_MESSAGE(false, "Source file: " << source_file_filename.string() << " has been deleted");
                it = dataset.RemoveSourceFile(it);
                continue;
            }

            sha512 source_sha512;
            source_sha512.generate(source_file_filename);

            auto source_checksum = source_sha512.getChecksumString();
            bool source_changed = file_info.source_file.sha512_checksum != source_checksum;
            BOOST_WARN(source_changed == false);

            bool outputs_changed = false;
            for (size_t i = 0; i < file_info.output_files.size(); ++i)
            {
                auto output_file_filename = file_info.output_files[i].filename;
                auto output_file_exists = std::filesystem::exists(output_file_filename);
                BOOST_WARN(output_file_exists);
                if (!output_file_exists)
                {
                    dataset.CancelChanged();
                    BOOST_CHECK_MESSAGE(false,
                                        "Output file: " << output_file_filename.string() << " does not exist - did source compile fail?");
                }

                sha512 output_sha512;
                output_sha512.generate(output_file_filename);
                auto output_checksum = output_sha512.getChecksumString();
                bool changed = file_info.output_files[i].sha512_checksum != output_checksum;
                BOOST_WARN(changed == false);
                outputs_changed |= changed;
                if (changed)
                {
                    BOOST_WARN_MESSAGE(false, "Checksum for output file: " << output_file_filename.string() << " has changed");
                    file_info.output_files[i].sha512_checksum = output_checksum;
                    dataset.SetChanged();
                }
            }

            if (source_changed && !outputs_changed)
            {
                std::string sep;
                std::string output_filenames("(");
                for (auto &output : file_info.output_files)
                {
                    output_filenames += sep;
                    output_filenames += dataset.StripFullPath(output.filename);
                    sep = ", ";
                }
                output_filenames += ")";

                BOOST_WARN_MESSAGE(false,
                                   "Updating checksum for source file: " << dataset.StripFullPath(source_file_filename)
                                                                         << " because it has changed, but checksums for outputs: "
                                                                         << output_filenames << " have not.");
                file_info.source_file.sha512_checksum = source_checksum;
                dataset.SetChanged();
            }
            else if (source_changed && outputs_changed)
            {
                BOOST_WARN_MESSAGE(false, "Updating checksums for source and outputs as both have changed.");
                file_info.source_file.sha512_checksum = source_checksum;
                dataset.SetChanged();
            }
            else if (!source_changed && outputs_changed)
            {
                dataset.CancelChanged();
                BOOST_CHECK_MESSAGE(false,
                                    "Output checksums have changed but source files: " << source_file_filename
                                                                                       << " has not. Has compiler changed?");
            }
            ++it;
        }
    }
}
