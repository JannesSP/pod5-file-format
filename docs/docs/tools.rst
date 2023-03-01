=====
Tools
=====


The ``pod5`` package provides the following tools for inspecting and manipulating
POD5 files as well as converting between `.pod5` and `.fast5` file formats.

.. contents:: Entry-Points
    :local:
    :depth: 1


Pod5 inspect
============

The `pod5 inspect` tool can be used to extract details and summaries of
the contents of `.pod5` files. There are two programs for users within `pod5 inspect`
and these are read and reads

.. code-block:: console

    $ pod5 inspect --help
    $ pod5 inspect {reads, read, summary} --help


pod5 inspect reads
------------------

Inspect all reads and print a csv table of the details of all reads in the given `.pod5` files.

.. code-block:: console

    $ pod5 inspect reads pod5_file.pod5

    read_id,channel,well,pore_type,read_number,start_sample,end_reason,median_before,calibration_offset,calibration_scale,sample_count,byte_count,signal_compression_ratio
    00445e58-3c58-4050-bacf-3411bb716cc3,908,1,not_set,100776,374223800,signal_positive,205.3,-240.0,0.1,65582,58623,0.447
    00520473-4d3d-486b-86b5-f031c59f6591,220,1,not_set,7936,16135986,signal_positive,192.0,-233.0,0.1,167769,146495,0.437
    ...

pod5 inspect read
-----------------

Inspect the pod5 file, find a specific read and print its details.

.. code-block:: console

    $ pod5 inspect read pod5_file.pod5 00445e58-3c58-4050-bacf-3411bb716cc3

    File: out-tmp/output.pod5
    read_id: 0e5d6827-45f6-462c-9f6b-21540eef4426
    read_number:    129227
    start_sample:   367096601
    median_before:  171.889404296875
    channel data:
    channel: 2366
    well: 1
    pore_type: not_set
    end reason:
    name: signal_positive
    forced False
    calibration:
    offset: -243.0
    scale: 0.1462070643901825
    samples:
    sample_count: 81040
    byte_count: 71989
    compression ratio: 0.444
    run info
        acquisition_id: 2ca00715f2e6d8455e5174cd20daa4c38f95fae2
        acquisition_start_time: 2021-07-23 13:48:59.780000
        adc_max: 0
        adc_min: 0
        context_tags
        barcoding_enabled: 0
        basecall_config_filename: dna_r10.3_450bps_hac_prom.cfg
        experiment_duration_set: 2880
        ...



pod5 merge
==========

`pod5 merge` is a tool for merging multiple  `.pod5` files into one monolithic pod5 file.

The contents of the input files are checked for duplicate read_ids to avoid
accidentally merging identical reads. To override this check set the argument
``-D / --duplicate_ok``

.. code-block:: console

    # View help
    $ pod5 merge --help

    # Merge a pair of pod5 files
    $ pod5 merge example_1.pod5 example_2.pod5 --output merged.pod5

    # Merge a glob of pod5 files
    $ pod5 merge *.pod5 -o merged.pod5

    # Merge a glob of pod5 files ignoring duplicate read ids
    $ pod5 merge *.pod5 -o merged.pod5 --duplicate_ok


pod5 filter
===========

`pod5 filter` is an alternative to `pod5 subset` where reads are subset from
one or more input `.pod5` files using a list of read ids provided using the `--ids` argument.

An important difference between `pod5 subset` and `pod5 filter` is that `--output`
specifies a directory in `subset` but a filepath in `filter`. This is because there is
only one output file in `pod5 filter`.

.. code-block:: console

    pod5 filter example.pod5 --output filtered.pod5 --ids read_ids.txt

The `--ids` filtering text file must be a simple list of valid UUID read_ids with
one read_id per line. The only valid exceptions are:

- Empty lines
- Trailing / Leading whitespace
- Lines beginning with a `#` (hash / pound symbol) to allow for comments
- The text `read_id` to allow for the header from `pod5 inspect reads`


pod5 subset
===========

`pod5 subset` is a tool for subsetting reads in `.pod5` files into one or more
output `.pod5` files. See also `pod5 filter`

The `pod5 subset` tool requires a *mapping* which defines which read_ids should be
written to which output. There are multiple ways of specifying this mapping which are
defined in either a `.csv` or `.json` file or by using a `--table` (csv or tsv)
and instructions on how to interpret it.

`pod5 subset` aims to be a generic tool to subset from multiple inputs to multiple outputs.
If your use-case is to `filter` read_ids from one or more inputs into a single output
then `pod5 filter` might be a more appropriate tool as the only input is a list of read_ids.

.. code-block:: console

    # View help
    $ pod5 subset --help

    # Subset input(s) using a pre-defined mapping
    $ pod5 subset example_1.pod5 --csv mapping.csv
    $ pod5 subset examples_*.pod5 --json mapping.json

    # Subset input(s) using a dynamic mapping created at runtime
    $ pod5 subset example_1.pod5 --table table.txt --columns barcode

.. important::

    Care should be taken to ensure that when providing multiple input `.pod5` files to `pod5 subset`
    that there are no read_id UUID clashes. If a duplicate read_id is detected an exception
    will be raised unless the `--duplicate_ok` argument is set. If `--duplicate_ok` is
    set then both reads will be written to the output, although this is not recommended.

Creating a Subset Mapping
------------------------------

The `.csv` or `.json` inputs should define a mapping of destination filename to an array
of read_ids which will be written to the destination.

Subset Mapping (.csv)
+++++++++++++++++++++++

The example below shows a `.csv` subset mapping. Note that the output filename can be
specified on multiple lines. This allows multi-line specifications to avoid excessively long lines.

.. code-block:: text

    output_1.pod5, 132b582c-56e8-4d46-9e3d-48a275646d3a, 12a4d6b1-da6e-4136-8bb3-1470ef27e311, ...
    output_2.pod5, 0ff4dc01-5fa4-4260-b54e-1d8716c7f225
    output_2.pod5, 0e359c40-296d-4edc-8f4a-cca135310ab2
    output_2.pod5, 0e9aa0f8-99ad-40b3-828a-45adbb4fd30c

Subset Mapping (.json)
+++++++++++++++++++++++++++

See below an example of a `.json` subset mapping. This file must of course be well-formatted
`json` in addition to the formatting standard required by the tool. The formatting requirements
for the `.json` mapping are that keys should be unique filenames mapped to an array
of read_id strings.

.. code-block:: json

    {
        "output_1.pod5": [
            "0000173c-bf67-44e7-9a9c-1ad0bc728e74",
            "006d1319-2877-4b34-85df-34de7250a47b"
        ],
        "output_2.pod5": [
            "00925f34-6baf-47fc-b40c-22591e27fb5c",
            "009dc9bd-c5f4-487b-ba4c-b9ce7e3a711e"
        ]
    }

Subset Mapping from Table
++++++++++++++++++++++++++++++++

`pod5 subset` can dynamically generate output targets and collect associated reads
based on a text file containing a table (csv or tsv) parsible by `pandas`.
This table file could be the output from `pod5 inspect reads` or from a sequencing summary.
The table must contain a header row and a series of columns on which to group unique
collections of values. Internally this process uses the
`pandas.Dataframe.groupby <https://pandas.pydata.org/docs/reference/api/pandas.DataFrame.groupby.html>`_
function where the `by` parameter is the sequence of column names specified with
the `--columns` argument.

Given the following example `--table` file, observe the resultant outputs given various
arguments:

.. code-block:: text

    read_id    mux    barcode      length
    read_a     1      barcode_a    4321
    read_b     1      barcode_b    1000
    read_c     2      barcode_b    1200
    read_d     2      barcode_c    1234

.. code-block:: console

    $ pod5 subset example_1.pod5 --output barcode_subset --table table.txt --columns barcode
    $ ls barcode_subset
    barcode-barcode_a.pod5     # Contains: read_a
    barcode-barcode_b.pod5     # Contains: read_b, read_c
    barcode-barcode_c.pod5     # Contains: read_d

    $ pod5 subset example_1.pod5 --output mux_subset --table table.txt --columns mux
    $ ls mux_subset
    mux-1.pod5     # Contains: read_a, read_b
    mus-2.pod5     # Contains: read_c, read_d

    $ pod5 subset example_1.pod5 --output barcode_mux_subset --table table.txt --columns barcode mux
    $ ls barcode_mux_subset
    barcode-barcode_a_mux-1.pod5    # Contains: read_a
    barcode-barcode_b_mux-1.pod5    # Contains: read_b
    barcode-barcode_b_mux-2.pod5    # Contains: read_c
    barcode-barcode_c_mux-2.pod5    # Contains: read_d

Output Filename Templating
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When subsetting using a table the output filename is generated from a template
string. The automatically generated template is the sequential concatenation of
`column_name-column_value` followed by the `.pod5` file extension.

The user can set their own filename template using the `--template` argument.
This argument accepts a string in the `Python f-string style <https://docs.python.org/3/tutorial/inputoutput.html#formatted-string-literals>`_
where the subsetting variables are used for keyword placeholder substitution.
Keywords should be placed within curly-braces. For example:

.. code-block:: console

    # default template used = "barcode-{barcode}.pod5"
    $ pod5 subset example_1.pod5 --output barcode_subset --table table.txt --columns barcode

    # default template used = "barcode-{barcode}_mux-{mux}.pod5"
    $ pod5 subset example_1.pod5 --output barcode_mux_subset --table table.txt --columns barcode mux

    $ pod5 subset example_1.pod5 --output barcode_subset --table table.txt --columns barcode --template "{barcode}.subset.pod5"
    $ ls barcode_subset
    barcode_a.subset.pod5    # Contains: read_a
    barcode_b.subset.pod5    # Contains: read_b, read_c
    barcode_c.subset.pod5    # Contains: read_d

Example subsetting from `pod5 inspect reads`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The `pod5 inspect reads` tool will output a csv table summarising the content of the
specified `.pod5` file which can be used for subsetting. The example below shows
how to split a `.pod5` file by the well field.

.. code-block:: console

    # Create the csv table from inspect reads, skipping the first line (File: ...)
    $ pod5 inspect reads example.pod5 | awk 'NR>1' > table.csv
    $ pod5 subset example.pod5 --table table.csv --columns well

Miscellaneous
~~~~~~~~~~~~~~

To disable the `tqdm <https://github.com/tqdm/tqdm>`_  progress bar set the environment
variable `POD5_PBAR=0`.

pod5 repack
===========

`pod5 repack` will simply repack `.pod5` files into one-for-one output files of the same name.

.. code-block:: console

    $ pod5 repack pod5s/*.pod5 repacked_pods/


pod5 convert fast5
=======================

The `pod5 convert fast5` tool takes one or more `.fast5` files and converts them
to one or more `.pod5` files.

.. warning::

    Some content previously stored in `.fast5` files is **not** compatible with the POD5
    format and will not be converted. This includes all analyses stored in the
    `.fast5` file.

.. important::

    The conversion of single-read fast5 files is not supported by this tool. Please
    first convert single-read fast5 files to multi-read fast5 files using the
    ont_fast5_api tools.

.. code-block:: console

    # View help
    $ pod5 convert fast5 --help

    # Convert fast5 files into a monolithic output file
    $ pod5 convert fast5 ./input/*.fast5 converted.pod5

    # Convert fast5 files into a monolithic output in an existing directory
    $ pod5 convert fast5 ./input/*.fast5 outputs/
    $ ls outputs/
    outputs/output.pod5 # default name

    # Convert each fast5 to its relative converted output. The output files are written
    # into the output directory at paths relatve to the path given to the
    # --output-one-to-one argument. Note: This path must be a relative parent to all
    # input paths.
    $ ls input/*.fast5
    file_1.fast5 file_2.fast5 ... file_N.fast5
    $ pod5 convert fast5 ./input/*.fast5 output_pod5s --output-one-to-one input/
    $ ls output_pod5s/
    file_1.pod5 file_2.pod5 ... file_N.pod5

    # Note the different --output-one-to-one path which is now the current working directory.
    # The new sub-directory output_pod5/input is created.
    $ pod5 convert fast5 ./input/*.fast5 output_pod5s --output-one-to-one ./
    $ ls output_pod5s/
    input/file_1.pod5 input/file_2.pod5 ... input/file_N.pod5

    # Convert all inputs so that they have neibouring pod5 files
    $ pod5 convert fast5 ./input/*.fast5 ./input/ --output-one-to-one ./input/
    $ ls input/*
    file_1.fast5 file_1.pod5 file_2.fast5 file_2.pod5  ... file_N.fast5 file_N.pod5


pod5 convert to_fast5
=====================

The `pod5 convert to_fast5` tool takes one or more `.pod5` files and converts them
to multiple `.fast5` files. The default behaviour is to write 4000 reads per output file
but this can be controlled with the `--file-read-count` argument.

.. code-block:: console

    # View help
    $ pod5 convert to_fast5 --help

    # Convert pod5 files to fast5 files with default 4000 reads per file
    $ pod5 convert to_fast5 example.pod5 pod5_to_fast5
    $ ls pod5_to_fast5/
    output_1.fast5 output_2.fast5 ... output_N.fast5
